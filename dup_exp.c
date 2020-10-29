#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
void change_control();

int main () {
	int console = dup(STDOUT_FILENO);
	int std_err = dup(STDERR_FILENO);
	change_control();
	dup2(console, STDOUT_FILENO);
	dup2(console, STDERR_FILENO);
	write(STDOUT_FILENO, "hello2", 6);
    return 0;
}

void change_control(){
	int fd = open("dup_file.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	char *args[] = {"ls", NULL};
	dup2(fd, STDOUT_FILENO);
	int rc = fork();
	if (rc < 0) {
		 // fork failed; exit
		 exit(1);
	}else if (rc == 0) {
		// child (new process)
		 if(execv("/bin/ls", args) == -1) {
			 exit(1);
		 }
		 write(STDERR_FILENO, "Error", 5);
	}else{
		wait(NULL); // hasta que no se ejecute el hijo no salimos
	
	}
	close(fd);
}