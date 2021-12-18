#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

mqd_t queue;
char* queue_name;

void PrintInfo(const char* q_name, struct mq_attr* q_inf) {
    printf("queue name:                %s\n", q_name);
    printf("queue flags :              %ld\n", q_inf->mq_flags);
    printf("Max message amount :       %ld\n", q_inf->mq_maxmsg);
    printf("Max message size (bytes) : %ld (%.1f pages)\n", q_inf->mq_msgsize, round(q_inf->mq_msgsize/4096));
    printf("Current message amount :   %ld\n", q_inf->mq_curmsgs);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage %s /queue_name\n", argv[0]);
        return 1;
    }
    queue_name = argv[1];

    // Открываем очередь с правами на посылку и получение сообщений - O_RDWR или создаём(если не существует ещё) - для этого O_CREAT
    queue = mq_open(queue_name, O_RDWR | O_CREAT, 0622, NULL);
    if (queue == (mqd_t) -1) {
        perror("mq_open");
        return 1;
    }

    //Получаем информацию про очередь
    struct mq_attr q_inf;
    mq_getattr(queue, &q_inf);

    long mq_msgsize = q_inf.mq_msgsize; //переменная, отвечающую за верхний предел размера сообщений, которые могут быть помещены в очередь
    char buf[mq_msgsize + 1];

    while (1) {

        size_t message = mq_receive(queue, buf, mq_msgsize, NULL); //количество байтов полученного сообщения
        if (message == (size_t) -1) { //Проверка ошибки
            perror("mq_receive"); 
            break;
        }

        buf[message] = '\n'; 
        if (write(STDOUT_FILENO, buf, message + 1) == -1) { //вывод сообщения в поток вывода
            perror("write"); //проверка ошибки
            break;
        }
    }  
    mq_unlink(argv[1]); //удаляет очередь сообщений
    // cleanup:
    mq_close(queue); //удаляет связь между дескриптором очереди сообщений, mqdes, и его очередью сообщений
    free(buf);
    return 0;
}
