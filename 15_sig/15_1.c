#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#define NUMBER_OF_SIGNALS 5

volatile int g_last_signal;
volatile siginfo_t *g_from_who;


const int signls[NUMBER_OF_SIGNALS] = {SIGINT, SIGQUIT, SIGTSTP, SIGHUP, SIGTERM};



//Обработчик сигналов
void sig_handler(int signum, siginfo_t *info, void *ucontext) {
  g_last_signal = signum;
  g_from_who = info;
  if (ucontext != NULL)
    ucontext = NULL;
}



int main(int argc, char *argv[]) {
	if(argc > 1) {
		printf("Too many arguments\n");
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	struct sigaction recieved = {};

  	recieved.sa_flags = SA_SIGINFO;
  	recieved.sa_sigaction = sig_handler;
	
	// Для убийства программы в параллельном терминале написать kill -9 <pid>
	// ps ux для получения pid процесса

	/*//Сигналы SIGKILL и SIGSTOP не могут быть пойманы, блокированы или проигнорированы
	if (signal(SIGINT, sig_handler) == SIG_ERR){ //SIGINT (прерывание программы) сигнал посылается, когда пользователь печатает INTR символ (обычно C-c). 
		perror("signal(SIGINT)"); //Функция signal() возвращает SIG_ERR при ошибке  
		return -1;
	} // ^c
	if (signal(SIGQUIT, sig_handler) == SIG_ERR) { //SIGQUIT управляется (символом QUIT), обычно C-\ и производит core-файл, когда он завершает процесс 
		perror("signal(SIGQUIT)");
		return -1;
	} // ^/
	if (signal(SIGTSTP, sig_handler) == SIG_ERR) { //Сигнал SIGTSTP - интерактивный сигнал останова. В отличие от SIGSTOP, этот сигнал может быть обработан и игнорироваться.
		perror("signal(SIGTSTP)");
		return -1;	
	} // ^Z 
	if (signal(SIGHUP, sig_handler) == SIG_ERR) { //SIGHUP ("зависание") сигнал используется, чтобы сообщить, что терминал пользователя разъединен
		perror("signal(SIGHUP)");
        	return -1;
	} 
	if (signal(SIGTERM, sig_handler) == SIG_ERR) { //сигнал, используемый, чтобы вызвать окончание программы. В отличие от SIGKILL, этот сигнал может быть блокирован, обрабатываться, и игнорироваться.
		perror("signal(SIGTERM)");
		return -1;
	} */

	//Реализация через цикл:



	for (int i = 0; i < NUMBER_OF_SIGNALS; i++) {
    		if (sigaction(signls[i], &recieved, NULL) < 0) {
      			perror("sigaction");
      			return -1;
    		}
  	}



    
	printf("to discover process pid, in another console use:\n \tps ux\n");
	printf("to kill programm, in another console use:\n \tkill -9 <pid>\n\n");
    
	while(1) {
		pause();
		printf("\tSignal %d came\n", g_last_signal);	
		
	}		
	return 0;
}
