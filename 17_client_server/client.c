#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage %s /queue_name 'message'\n", argv[0]); //При неверном входе - ошибка
        return 1;
    }

    //Создаём очередь
    mqd_t queue = mq_open(argv[1], O_WRONLY, NULL); // O_WRONLY -> можем только посылать сообщения
    if (queue == (mqd_t) -1) {
        perror("mq_open"); //В случае ошибки, mq_open() возвращает (mqd_t) -1
        return 1;
    }

    // Вывод сообщения
    if (mq_send(queue, argv[2], strlen(argv[2]), 0) == -1) { //добавляет сообщение, на которое указывает msg_ptr, в очередь сообщений, на которую ссылается дескриптор mqdes
        perror("mq_send"); 
    }

    // cleanup:
        mq_unlink(argv[1]); //удаляет очередь сообщений
    // cleanup:
    mq_close(queue); //удаляет связь между дескриптором очереди сообщений, mqdes, и его очередью сообщений
    free(buf);
    return 0;
}
