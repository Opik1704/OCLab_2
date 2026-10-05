#include <windows.h>
#include <stdio.h>

#define MAX_PATH_LEN 256
#define BUF_SIZE 512

void PrintError(const char* msg) {
    DWORD errCode = GetLastError();
    printf("%s Error Code: %lu\n", msg, errCode);
}

int main(void) {
    char filename[MAX_PATH_LEN];

    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.lpSecurityDescriptor = NULL;
    saAttr.bInheritHandle = TRUE;

    HANDLE InputFile = CreateFileA("input.txt", GENERIC_READ, FILE_SHARE_READ, &saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (InputFile == INVALID_HANDLE_VALUE) {
        printf("Cant open file Code: %lu\n", GetLastError());
        system("pause");
        return 1;
    }

    HANDLE pipeRead = NULL;
    HANDLE pipeWrite = NULL;

    if (!CreatePipe(&pipeRead, &pipeWrite, &saAttr, 0)) {
        printf("Cant create pipe Code Erore: %lu\n", GetLastError());
        CloseHandle(InputFile);
        system("pause");
        return 1;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;

    si.hStdInput = InputFile;  
    si.hStdOutput = pipeWrite; 

    si.hStdError = GetStdHandle(STD_ERROR_HANDLE); 

    BOOL success = CreateProcessA("child.c.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

    if (!success) {
        PrintError("Cant start ");
        CloseHandle(InputFile);
        CloseHandle(pipeRead);
        CloseHandle(pipeWrite);
        system("pause");

        return 1;
    }

    CloseHandle(InputFile); 
    CloseHandle(pipeWrite);

    //терь читаем из пайпа

    char buffer[BUF_SIZE];
    DWORD bytesRead = 0;

    printf("Results:\n");
    while (ReadFile(pipeRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        if (exitCode == 2) {
            printf("\nChild process reported\n");
        }

    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(pipeRead);

    printf("\n Enter for exit");
    system("pause");

    return 0;
}


