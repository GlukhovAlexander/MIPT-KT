#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

//Для организации взаимоисключения на критических участках используем семафор mutex
//

typedef struct {
	pthread_mutex_t mutex;
	unsigned long long counter;
	int iterations;
} work_area_t;

//
void * thr_body(void * arg) {
	work_area_t * w = arg;
	for (int i = 0; i < w->iterations; i++) {
		pthread_mutex_lock(&w->mutex); //Объект, на который ссылается mutex блокируется
		w->counter++;
		pthread_mutex_unlock(&w->mutex);
	}

	puts("Done (secondary)");
	return NULL;
}

int main(int argc, char * argv[]) {
	
	//PTHREAD_MUTEX_INITIALIZER - создание mutex'a с "базовыми настройками"
	work_area_t data = {
		.mutex = PTHREAD_MUTEX_INITIALIZER,
		.counter = 0,
		.iterations = (argc > 1) ? atoi(argv[1]) : 1000,
	};

	pthread_t secondary_thread_id; //возвращает положительное значение, если ошибка, 0, если успех
	if (errno = pthread_create(&secondary_thread_id, NULL, thr_body, &data)) { //эта функция запускает новый поток в вызывающем процессе новый поток начинает выполнение с вызова *start_routine, в данном случае thr_body &data передаётся в качетве единственного аргумента
		perror("pthread_create");
		return 1;
	}

	for (int i = 0; i < data.iterations; i++) {
		pthread_mutex_lock(&data.mutex);
		data.counter++;
		pthread_mutex_unlock(&data.mutex);
	}

	puts("Done (main)");
	//нить исполнения, вызвавшая эту функцию переходит в состояние ожидания до завершения thread'a
	pthread_join(secondary_thread_id, NULL);
	return 0;
}
