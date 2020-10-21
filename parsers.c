#include "parsers.h"

// returns [command, other stuff, NULL]
char **parse_input(char *input, char *separator){
	char** parsed_input = (char**)calloc(1, sizeof(char*));
	char* parse_part;
	int index = 0;
	while((parse_part = strsep(&input, separator)) != NULL ){
		if(strlen(parse_part) > 0){
			parsed_input[index] = parse_part;
			index += 1;
			parsed_input = (char**)realloc(parsed_input, (index+1)*sizeof(char*));
		}
	}
	parsed_input[index] = NULL;
	return parsed_input;
}

// Returns [[command, other stuff, NULL], [outputfile, NULL]] or  NULL if there a mistakes in the input  
char ***parse_input_redir(char* input){
	char ***parsed_input = NULL;
	char *left_side = strsep(&input, TEXT_SEPARATOR_REDIRR);
	char *right_side = input;
	char **parsed_left_side = parse_input(left_side, TEXT_SEPARATOR);
	if(parsed_left_side[0] == NULL)
		return parsed_input;
	char **parsed_right_side = parse_input(right_side, TEXT_SEPARATOR);
	if(parsed_right_side[0] == NULL || strcmp(parsed_right_side[0], TEXT_SEPARATOR_REDIRR) == 0 || parsed_right_side[1] != NULL)
		return parsed_input;
	parsed_input = (char***)malloc(2*sizeof(char**));
	parsed_input[0] = parsed_left_side;
	parsed_input[1] = parsed_right_side;
	return parsed_input;
}

// Returns [[command0, other stuff0, NULL], ..., [commandn, other stuffn, NULL], NULL]
char ***parse_input_parall(char* input){
	char ***parsed_input = (char***)malloc(sizeof(char**));
	char **parsed_input_apersan = parse_input(input, TEXT_SEPARATOR_PARALLEL);
	char **parsed_command;
	int index = 0;
	for(char **p = parsed_input_apersan; *p; p++){
		parsed_command = parse_input(*p, TEXT_SEPARATOR);
		parsed_input[index] = parsed_command;
		index ++;
		parsed_input = (char***)realloc(parsed_input, (index+1)*sizeof(char**));
	}
	parsed_input[index] = NULL;
	return parsed_input;
}