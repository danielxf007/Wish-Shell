#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include <sys/wait.h>
#include "parsers.h"
#include <fcntl.h>
#define ERROR_MESSAGE "An error has occurred\n"
char **search_path;
void inter_mode();
void bash_mode(char* file_name);
int get_input_type(char ***input);
void cd(char* path_name);
void path(char **args);
char* get_path(char* command);
int get_n_commands(char ***parsed_input_parall);
char **copy_command(int start, int end, char **command);
void execute_command(char **args);
void execute_command_redir(char **expr, int index);
void execute_commands(int input_type, char ***parsed_expr);

int get_index(char *str, char c){
	int index = -1;
	if(str){
		for(char *p = str; *p; p++){
			if(*p == c){
				index = p-str;
				break;
			}
		}
	}
	return index;
}

int get_index_2d(char **arr, char *element){
	int index = -1;
	if(arr){
		for(char **p = arr; *p; p++){
			if(!strcmp(*p, element)){
				index = p-arr;
				break;
			}
		}
	}
	return index;
}

int main(int argc, char** argv){
	char *bin = "/bin";
	search_path = (char**)malloc(2*sizeof(char*));
	search_path[0] = bin;
	search_path[1] = NULL;
	switch(argc-1){
		case 0:
			inter_mode();
			break;
		case 1:
			bash_mode(argv[1]);
			break;
		default:
			write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
			exit(1);
	}
	return 0;
}

void inter_mode(){
	char *input;
	char *command_input;
	char ***parsed_expr;
	size_t len;
	int input_type;
	int char_index;
	while(1){
		input = NULL;
		len = 0;
		printf("wish> ");
		getline(&input, &len, stdin);
		char_index = get_index(input, END_LINE);
		command_input = sub_str(0, char_index, input);
		parsed_expr = parse_in(command_input);
		input_type = get_input_type(parsed_expr);
		execute_commands(input_type, parsed_expr);
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
	char *command_input;
	char ***parsed_expr;
	int char_index;
	size_t len = 0;
	int input_type;
	while (getline(&input, &len, file) != -1){
		char_index = get_index(input, END_LINE);
		command_input = sub_str(0, char_index, input);
		parsed_expr = parse_in(command_input);
		input_type = get_input_type(parsed_expr);
		execute_commands(input_type, parsed_expr);
	}
	fclose(file);
	exit(0);
}

int get_input_type(char ***input){
	int input_type = 4;
	if(input == NULL)
		input_type = 0;
	else if(input[0][0] == NULL)
		input_type = 1;
	else if(input[1] != NULL)
		input_type = 2;
	else if(get_index_2d(input[0], ">\0") != -1)
		input_type = 3;
	return input_type;
}

void cd(char* path_name){
	if(chdir(path_name) == -1) write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
}

void path(char **args){
	if (search_path != NULL) free(search_path);
	search_path = (char**)malloc(sizeof(char*));
	char *path_name = NULL;
	int index = 0;
	char **p = args;
	while(*(++p)){
		path_name = (char*)malloc(strlen(*p)*sizeof(char));
		stpcpy(path_name, *p);
		search_path[index] = path_name;
		index++;
		search_path = (char**)realloc(search_path, (index+1)*sizeof(char*));
	}
	search_path[index] = NULL;
}

char *get_path(char *command){
	char *path_name = NULL;
	if(!search_path) return path_name;
	for(char **p = search_path; *p; p++){
		path_name = (char*)malloc((strlen(*p) + strlen(command)+1)*sizeof(char));
		stpcpy(path_name, *p);
		strcat(path_name, "/");
		strcat(path_name, command);
		if(access(path_name, X_OK) == 0) break;
		else {
			free(path_name);
			path_name = NULL;
		}
	}
	return path_name;
}


int get_n_commands(char ***parsed_input_parall){
	int n_commands = 0;
	char ***commands = parsed_input_parall;
	while(*commands){
		n_commands++;
		commands++;
	}
	return n_commands;
}

char **copy_command(int start, int end, char **command){
	char **new_command = (char**)malloc((end-start+1)*sizeof(char*));
	for(int i = start; i < end; i++)
		new_command[i-start] = command[i];
	new_command[end-start] = NULL;
	return new_command;
	
}

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
			int rc = fork();
			if (rc == 0) {
				 if(execv(path_name, args) == -1) {
					 write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
					 exit(1);
				 }				 
			}else
				wait(NULL);
		}else write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
	}
}

void execute_command_redir(char **expr, int index){
	char **args = copy_command(0, index-1, expr);
	int fd = open(expr[index-1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	int std_out = dup(STDOUT_FILENO);
	int std_err = dup(STDERR_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	execute_command(args);
	close(fd);
	dup2(std_out, STDOUT_FILENO);
	dup2(std_err, STDERR_FILENO);			
}

void execute_command_parall(char ***expr){
	int n_commands = get_n_commands(expr);
	int index_c;
	pid_t *pids = (pid_t*) malloc(n_commands*sizeof(pid_t));
	int index;
	for(index= 0; index < n_commands; index++){
		if((pids[index]= fork()) == 0){
			if(expr[index][0] == NULL)
				exit(0);
			index_c = get_index_2d(expr[index], ">\0");
			if(index_c != -1)
				execute_command_redir(expr[index], index_c);
			else
				execute_command(expr[index]);
			exit(0);
		}
	}
	while(n_commands--)
		wait(NULL);
}

void execute_commands(int input_type, char ***parsed_expr){
	int index;
	switch(input_type){
		case 0:
			write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)*sizeof(char));
			break;
		case 1:
			break;
		case 2:
			execute_command_parall(parsed_expr);
			break;
		case 3:
			index = get_index_2d(parsed_expr[0], ">\0");
			execute_command_redir(parsed_expr[0], index);
			break;
		default:
			execute_command(parsed_expr[0]);
	}
}