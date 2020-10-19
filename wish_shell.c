#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#define ERROR_MESSAGE "An error has occurred\n"
#define TEXT_SEPARATOR " "
#define END_OF_LINE "\n"
void inter_mode();
void bash_mode(char* file_name);
void cd(char* arg);
void path(char** args);
void print_2_d_char_arr(char** arr);
char** parse_input(char* input);
void execute_command(char** args);
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
	char *line;
	size_t len;
	ssize_t n_ch_read;
	char** parsed_input;

	while(!finished){
		line = NULL;
		len = 0;
		printf ("wish> ");
		n_ch_read = getline(&line, &len, stdin);
		parsed_input = parse_input(strsep(&line, END_OF_LINE));
		execute_command(parsed_input);
		free(parsed_input);
		if(n_ch_read < 2) break;	
	}
}

void bash_mode(char* file_name){
	FILE *file;
	file = fopen(file_name, "r");
	if(file == NULL){
        fprintf( stderr,"%s", ERROR_MESSAGE);
        exit(1);
    }
	char *line = NULL;
	size_t len = 0;
	ssize_t n_ch_read;
	while ((n_ch_read = getline(&line, &len, file)) != -1){
	}
	exit(0);
}

// Built in Commands

void cd(char* arg){
	printf("%s \n", "CD");
}

void path(char** args){
	printf("%s \n", "PATH");
}

// Parser returns [command, other stuff, NULL]

char** parse_input(char* input){
	char** parsed_input = (char**)calloc(1, sizeof(char*));
	char* parse_part;
	int index = 0;
	while((parse_part = strsep(&input, TEXT_SEPARATOR)) != NULL ){
		parsed_input[index] = parse_part;
		index += 1;
		parsed_input = (char**)realloc(parsed_input, (index+1)*sizeof(char*));
	}
	parsed_input[index] = NULL;
	return parsed_input;
}

// Useful

void print_2_d_char_arr(char** arr){
	for(char** p = arr; *p != NULL; p++){
		printf("%s \n", *p);
	}
}

//

void execute_command(char** args){
	char* exit_custom = "exit";
	char* cd_custom = "cd";
	char* path_custom = "path";
	if(strcmp(args[0], exit_custom) == 0){
		if(args[1] == NULL) exit(0);
		else fprintf( stderr,"%s", ERROR_MESSAGE);		
	}else if(strcmp(args[0], cd_custom) == 0){
		if (args[1] != NULL && args[2] == NULL) cd(args[1]);
		else fprintf( stderr,"%s", ERROR_MESSAGE);
	}else if(strcmp(args[0], path_custom) == 0){
		path(args);
	}else{
		printf("%s \n", "Other Commands");
	}
}

