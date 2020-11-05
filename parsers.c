#include "parsers.h"

char *sub_str(int start, int end, char *str){
	char *new_str = (char*)malloc((end-start+1)*sizeof(char));
	new_str[end-start] = '\0';
	strncpy(new_str, str, end-start);
	return new_str;
}

char *add_element(char element, int *stack_index, char *stack){
	char *new_stack;
	(*stack_index)++;
	new_stack = (char*)realloc(stack, (*stack_index+1)*sizeof(char));
	new_stack[*stack_index] = element;
	return new_stack;
}

char *clear_stack(int *stack_index){
	char *new_stack;
	*stack_index = 0;
	new_stack = (char*)malloc(sizeof(char));
	new_stack[*stack_index] = '\0';
	return new_stack;
}

char **add_arg(int stack_index, char *stack, int *index_c, char **command){
	char **new_command;
	char *new_arg = (char*)malloc((stack_index+1)*sizeof(char));
	for(int i = 0; i <= stack_index; i++)
		new_arg[i] = stack[i];
	command[*index_c] = new_arg;
	(*index_c)++;
	new_command = (char**)realloc(command, ((*index_c)+1)*sizeof(char*));
	new_command[*index_c] = NULL;
	return new_command;
	
}

char ***add_command(char **command, int *index_l, char ***command_list){
	char ***new_command_list;
	command_list[*index_l] = command;
	(*index_l)++;
	new_command_list = (char***)realloc(command_list, ((*index_l)+1)*sizeof(char**));
	new_command_list[*index_l] = NULL;
	return new_command_list;
}

char ***parse_in(char *input){
	char ***command_list = (char***)malloc(sizeof(char**));
	command_list[0] = NULL;
	char **command = (char**)malloc(sizeof(char*));
	command[0] = NULL;
	char *stack = (char*)malloc(sizeof(char));
	stack[0] = '\0';
	int index_c = 0, index_l = 0, stack_index = 0, redir_flag = 0;
	for(char *p = input; *p; p++){
		if(*p == TEXT_SEPARATOR || *p == TEXT_TAB){
			if(stack[stack_index] == '\0' || stack[stack_index] == TEXT_SEPARATOR_REDIRR)
				continue;
			else if(stack[stack_index] == TEXT_SEPARATOR_REDIRR && command[0] == NULL)
				return NULL;
			else{
				command = add_arg(stack_index, stack, &index_c, command);
				stack = clear_stack(&stack_index);
			}
				
		}else if(*p == TEXT_SEPARATOR_REDIRR){
				if(redir_flag || stack[stack_index] == TEXT_SEPARATOR_REDIRR || (stack[stack_index] == '\0' && command[0] == NULL))
					return NULL;
				else if(command[0] != NULL)
					stack[stack_index] = TEXT_SEPARATOR_REDIRR;
				else{
					command = add_arg(stack_index, stack, &index_c, command);
					stack = clear_stack(&stack_index);
					stack[stack_index] = TEXT_SEPARATOR_REDIRR;
				}
				redir_flag = 1;
		}else if(*p == TEXT_SEPARATOR_PARALLEL){
				if(stack[stack_index] == TEXT_SEPARATOR_REDIRR)
					return NULL;
				else if(redir_flag){
					if(stack[stack_index] != '\0')
						command = add_arg(stack_index, stack, &index_c, command);
					command[index_c] = ">\0";
					index_c++;
					command = (char**)realloc(command, (index_c+1)*sizeof(char*));
					command[index_c] = NULL;					
				}else if(stack[stack_index] != '\0')
					command = add_arg(stack_index, stack, &index_c, command);
				command_list = add_command(command, &index_l, command_list);
				command = (char**)malloc(sizeof(char*));
				index_c = 0;
				command[index_c] = NULL;
				stack = clear_stack(&stack_index);
				redir_flag = 0;
		}else{
			if(redir_flag && stack[stack_index] == '\0')
				return NULL;
			else if(stack[stack_index] == '\0' || stack[stack_index] == TEXT_SEPARATOR_REDIRR)
				stack[stack_index] = *p;
			else
				stack = add_element(*p, &stack_index, stack);
		}
	}
	if(stack[stack_index] == TEXT_SEPARATOR_REDIRR)
		return NULL;
	else if(redir_flag){
			if(stack[stack_index] != '\0')
				command = add_arg(stack_index, stack, &index_c, command);
			command[index_c] = ">\0";
			index_c++;
			command = (char**)realloc(command, (index_c+1)*sizeof(char*));
			command[index_c] = NULL;
	}else if(stack[stack_index] != '\0')
		command = add_arg(stack_index, stack, &index_c, command);
	command_list = add_command(command, &index_l, command_list);
	return command_list;
}