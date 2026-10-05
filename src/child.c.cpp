#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int main(void) {
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), stdin) != NULL) {

        if (strlen(line) == 0) {
            continue;
        }

        int first_num;
        int offset = 0;

        //Считываем первое число и запоминаем, сколько символов прочитано 
        if (sscanf_s(line, "%d%n", &first_num, &offset) != 1) {
            continue; // Пропускаем пустые строки или строки без чисел
        }

        int result = first_num;
        int next_num;
        int bytes_read = 0;
        int div_by_zero = 0;

        // Сдвигаем указатель на считанный кусок и читаем следующие числа одно за другим
        char* ptr = line + offset;

        while (sscanf_s(ptr, "%d%n", &next_num, &bytes_read) == 1) {
            if (next_num == 0) {
                div_by_zero = 1;
                break;
            }
            result /= next_num;
            ptr += bytes_read; 
        }
        if (div_by_zero) {
            fprintf(stderr, "Error division by zero  0! \n");
            //Что делает : Выводит текстовое сообщение об ошибке в стандартный поток ошибок stderr.
            fflush(stderr);
            //Что делает: Принудительно очищает буфер потока stderr и выталкивает данные в систему.
            exit(2);
            //Аварийно завершает выполнение дочернего процесса и передает операционной системе код возврата 2.
            //Родительский процесс, ожидающий дочерний через системный вызов WaitForSingleObject 
            //просыпается, читает код 2 видит, что в дочернем процессе произошло деление на 0 после чего завершает работу
        }
        printf("%d\n", result);
        fflush(stdout);
        //Что делает: Принудительно выталкивает все накопившиеся результаты вычислений из буфера stdout в канал IPC
    }

    return 0;
}