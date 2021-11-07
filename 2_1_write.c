//записать в указанный файл указанное содержимое (строку)

#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdint.h> 
#include <unistd.h> 

ssize_t write_all(int fd, const void *buf, size_t count) 
{
	size_t bytes_written = 0; 
	const uint8_t *buf_adder = buf; 
	
//пока сумма прочитанных байтов не станет равна длине строки, читаем
	while (bytes_written < (size_t) count) 
	{
		ssize_t res = write(fd, buf_adder + bytes_written, count - bytes_written); 
		if(res < 0) 
			return res; 
		bytes_written += res; 
			
	}
	return (ssize_t)bytes_written; 
}

int main(int argc, char * argv[]) 
{
//Выход с ошибкой, если неправильный ввод
	if (argc != 3) 
	{
		fprintf(stderr, "Usage: %s path text", argv[0]); 
		return 1; 
	}

//Если файл для записи не открывается тоько для чтения, ...
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); 

//Выход с ошибкой, если файл не окрылся для чтения
	if (fd < 0) 
	{
		perror("Failed to open file for writing"); //вывод в поток ошибок
		return 2;
	}

//Выход с ошибкой, если 
	if (write_all(fd, argv[2], strlen(argv[2])) < 0)
	{
		perror("Failed to write"); 
		close(fd); 
		return 3;
	}

//Выход с ошибкой, если файл не закрылся
	if (close(fd) < 0) 
	{
		perror("Failure during close"); 
		return 4; 
	}

	return 0; 
}
