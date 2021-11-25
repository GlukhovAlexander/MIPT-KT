//13.2 программа порождает дочерний процесс, он дожидается завершения родительского процесса parent pid на 1 меньше текущего, когда родитель жив, когда parent умирает, pid дочернего меняется

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	pid_t pid;
	
	pid = fork(); //создали дочерний процесс

	switch(pid) {
		case -1:
			perror("fork failed"); //Вывод в случае ошибки при создании

			exit(1); 
		case 0: //дочерний процесс
			printf("child\t%d\t%d\t%d\n", getpid(), getppid(), getpgid(getpid()));
			sleep(3);
			printf("child process finished\n");
			exit(0); 
		default: //родительский процесс
			printf("Parent\t%d\t%d\t%d\n", getpid(), getppid(), getpgid(getpid()));
			printf("parent process finished\n");
			exit(0);
	}
	return 0;
}

