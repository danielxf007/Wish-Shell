#ifndef PARSERS_H
#define PARSERS_H
#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#define TEXT_SEPARATOR ' '
#define TEXT_TAB '\t'
#define TEXT_SEPARATOR_REDIRR '>'
#define TEXT_SEPARATOR_PARALLEL '&'
#define END_LINE '\n'
char *sub_str(int start, int end, char *str);
char **add_arg(int stack_index, char *stack, int *index_c, char **command);
char *add_element(char element, int *stack_index, char *stack);
char *clear_stack(int *stack_index);
char ***add_command(char **command, int *index_l, char ***command_list);
char ***parse_in(char *input);
#endif