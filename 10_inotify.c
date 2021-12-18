//Код из мана
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define SIZEOF_BUF  4096
//проходим по всем событиям, которые можем прочитать из файлового дескриптора 
void handle_event(int fd, int wd, char* pathname) { 
    char buf[SIZEOF_BUF];
    const struct inotify_event *event;
    ssize_t len;
    char *ptr;
    for(;;) {
        len = read(fd, buf, sizeof(buf));
        if(len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }
//Если неблокирующий read() не найдёт событий для чтения, то вернёт -1 с errno равным EAGAIN, тогда выходим из цикла
        if(len <= 0) {
            break;
        }
//проходим по всем событиям в буфере
        for(ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event*)buf;
// печатаем тип события
            if(event->mask & IN_CREATE) {
                printf("IN_CREATE: ");
            }

	    if((event->cookie != 0) && (event -> mask & IN_MOVE)) {
		printf("IN_MOVE (rename): ");
        }

        if(wd == event->wd) {
            printf("%s/", pathname);
        }
        if(event->len) {
            printf("%s", event->name);
        }

        if(event->mask & IN_ISDIR) {
            printf(" [directory]\n");
        } else {
            printf(" [file]\n");
        }
    }
}
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Need 2 arguments");
        exit(EXIT_FAILURE);
    }

    char buf;
    int fd, poll_num, wd;
    nfds_t nfds;
    struct pollfd fds[2];
// Создаём файловый дескриптор для доступа к inotify API
    fd = inotify_init1(IN_NONBLOCK);
    if(fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }
// выделяем память под дескрипторы 
    wd = inotify_add_watch(fd, argv[1], IN_CREATE| IN_MOVE);
    if(wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    nfds = 2;
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = fd;
    fds[1].events = POLLIN;
    printf("Listening for events.\n");
//ждём события и/или ввода с терминала 
    while(1) {
        poll_num = poll(fds, nfds, -1);
        if(poll_num == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            exit(EXIT_FAILURE);
        }
        if(poll_num > 0) {
            if (fds[0].revents & POLLIN) {
//опустошаем stdin и выходим  
      		    while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }
	    if (fds[1].revents & POLLIN) {
//доступны события
    		    handle_event(fd, wd, argv[1]);
            }
        
	}
    }

    printf("Listening for events stopped.\n");
    close(fd);
    return 0;
}
