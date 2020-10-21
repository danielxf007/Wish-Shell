#ifndef PARSERS_H
#define PARSERS_H
#include<string.h>
#include <stdlib.h>
#define TEXT_SEPARATOR " "
#define TEXT_SEPARATOR_REDIRR ">"
#define TEXT_SEPARATOR_PARALLEL "&"

char **parse_input(char *input, char *separator);
char ***parse_input_redir(char* input);
char ***parse_input_parall(char* input);

#endif