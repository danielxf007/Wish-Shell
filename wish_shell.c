#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include <sys/wait.h>
#include "parsers.h"
#include <fcntl.h>
#define ERROR_MESSAGE "An error has occurred\n"
#define END_OF_LINE "\n"
char **search_path = NULL;
void inter_mode();
void bash_mode(char* file_name);
int get_input_type(char *input);
void cd(char* path_name);
void path(char **args);
char* get_path(char* command);
int get_n_commands(char ***parsed_input_parall);
void execute_command(char **args);
void execute_command_redir(char **args, char *file_name);
void execute_commands(int input_type, char *input);
void print_2_d_arr(char **arr);

int main(int argc, char** argv){
	
	switch(argc-1){
		case 0:
			inter_mode();
			break;
		case 1:
			bash_mode(argv[1]);
			break;
		default:
			fprintf( stderr,"%s", ERROR_MESSAGE);
			exit(1);
	}
	return 0;
}
//Sheel modes
void inter_mode(){
	int finished = 0;
	char *input;
	size_t len;
	ssize_t n_ch_read;
	int input_type;
	while(!finished){
		input = NULL;
		len = 0;
		printf("wish> ");
		n_ch_read = getline(&input, &len, stdin);
		input_type = get_input_type(input);
		execute_commands(input_type, input);
	}
}

void bash_mode(char* file_name){
	FILE *file;
	file = fopen(file_name, "r");
	if(file == NULL){
        write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
        exit(1);
    }
	char *input = NULL;
	size_t len = 0;
	ssize_t n_ch_read;
	int input_type;
	pid_t pid;
	while ((n_ch_read = getline(&input, &len, file)) != -1){
		input_type = get_input_type(input);
		execute_commands(input_type, input);
		sleep(1);
	}
	fclose(file);
	exit(0);
}

// Returns the type of input, 0 for Redirection, 1 for Parallel, 2 for normal

int get_input_type(char *input){
	int input_type = 0;
	if(strstr(input, TEXT_SEPARATOR_REDIRR)) input_type = 1;
	else if(strstr(input, TEXT_SEPARATOR_PARALLEL)) input_type = 2;
	return input_type;
}

// Built in Commands

void cd(char* path_name){
	if(chdir(path_name) == -1) fprintf( stderr,"%s", ERROR_MESSAGE);
}

void path(char **args){
	if (search_path != NULL) free(search_path);
	search_path = (char**)malloc(sizeof(char*));
	int index = 0;
	char **p = args;
	while(*(p++)){
		search_path[index] = *p;
		index++;
		search_path = (char**)realloc(search_path, (index+1)*sizeof(char*));
	}
	search_path[index] = NULL;
}

// Return 1 if the command can be executed, 0 otherwise

char *get_path(char *command){
	char *path_name = NULL;
	if(!search_path) return path_name;
	for(char **p = search_path; *p; p++){
		path_name = (char*)malloc((strlen(*p) + strlen(command))*sizeof(char));
		stpcpy(path_name, *p);
		strcat(path_name, command);
		if(access(path_name, X_OK) == 0) break;
		else {
			free(path_name);
			path_name = NULL;
		}
	}
	return path_name;
}

//
int get_n_commands(char ***parsed_input_parall){
	int n_commands = 0;
	char ***commands = parsed_input_parall;
	while(*commands){
		n_commands++;
		commands++;
	}
	return n_commands;
}
//

void execute_command(char **args){
	char* exit_custom = "exit";
	char* cd_custom = "cd";
	char* path_custom = "path";
	if(strcmp(args[0], exit_custom) == 0){
		if(args[1] == NULL) exit(0);
		else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));	
	}else if(strcmp(args[0], cd_custom) == 0){
		if (args[1] != NULL && args[2] == NULL) cd(args[1]);
		else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
	}else if(strcmp(args[0], path_custom) == 0){
		path(args);
	}else{
		char *path_name = get_path(args[0]);
		if(path_name != NULL){
			/*
			int rc = fork();
			 if (rc < 0) {
				 // fork failed; exit
				 write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
				 exit(1);
			}else if (rc == 0) {
				// child (new process)
				 if(execv(path_name, args) == -1) {
					 printf("%s", "here");
					 write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
					 exit(1);
				 }				 
			}else{
				wait(NULL); // hasta que no se ejecute el hijo no salimos
			}
			*/
		}//else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
	}
}
//
void print_2_d_arr(char **arr){
	if(arr != NULL){
		for(char **p = arr; *p; p++){
			printf("%s \n", *p);
	}
	}

}
//
void execute_commands(int input_type, char *input){
	print_2_d_arr(search_path);
	char ***parsed_input_redir;
	char ***parsed_input_parall;
	char **parsed_input;
	switch(input_type){
		case 1:
			parsed_input_redir = parse_input_redir(strsep(&input, END_OF_LINE));
			if(parsed_input_redir){
				int fd = open(parsed_input_redir[1][0], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
				int std_out = dup(STDOUT_FILENO);
				int std_err = dup(STDERR_FILENO);
				dup2(fd, STDOUT_FILENO);
				dup2(fd, STDERR_FILENO);
				execute_command(parsed_input_redir[0]);
				close(fd);
				dup2(std_out, STDOUT_FILENO);
				dup2(std_err, STDERR_FILENO);
				free(parsed_input_redir);					
			}else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
			break;
		case 2:
			parsed_input_parall = parse_input_parall(strsep(&input, END_OF_LINE));
			if(parsed_input_parall){
				int n_commands = get_n_commands(parsed_input_parall);
				pid_t *pids = (pid_t*) malloc(n_commands*sizeof(pid_t));
				int index;
				for(int index= 0; index < n_commands; index++){
					if((pids[index]= fork()) == 0){
						execute_command(parsed_input_parall[index]);
						exit(0);
					}
				}
				while(n_commands--)
					wait(NULL);
				free(pids);
				free(parsed_input_parall);
			}else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
			break;
		default:
			parsed_input = parse_input(strsep(&input, END_OF_LINE), TEXT_SEPARATOR);
			if(parsed_input){
				execute_command(parsed_input);
				free(parsed_input);
			}else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
	}	
}