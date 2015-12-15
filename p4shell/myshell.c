#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>


/* Some constants */
#define MAX_CMD_LENGTH 512
char error_message[30] = "An error has occurred\n";

typedef struct node {
	char *token;
	struct node *next;
} node_t;

/************************
 * Function definitions *
 ************************/
void myPrint(char *msg); // for all printing to screen
// related to linked-list for storing cmd tokens
void create_node(node_t **head, char *token);
void clear_node(node_t *node);
node_t *parse_command_to_node(char *cmd, char *delim);
int node_length(node_t *node);
// parse single command line and store arguments into an array
char **parse_single_command(char *cmd, char *delim); 
// for build-in commands
void buildin_cd(char **args);
char *buildin_pwd(char *cwd);
void buildin_exit(char **args);
// execute command functions (not build-in)
void execute_process(char **args);
// check for build-ins and execute command
void execute_command(char **args);
// run the shell process with parsed command arguments
void run_shell(char **args);
// multiple command
void run_multiple_command(node_t *node, char **args, char *delim_m, char *delim_r);
int is_empty(char *s);
void execute_redirection(char **args, char *red_file);
char *remove_whitespace(char *name);
int count_occur(char *cmd);

/*****************
 * Main function *
 *****************/
int main(int argc, char *argv[]) 
{
    char cmd_buff[MAX_CMD_LENGTH+2]; // adding newline and null-termination char
    char *pinput; // command line
	char **args;  // args as tokens
    char **test_args;
    char **red_args;
    char *red_file;
	char *delim = ";";
	char *delim_m = " \t\r\n\a";
	char *delim_r = ">";
	node_t *sin_cmd = NULL;
	
	// batch mode
	if ((argc == 2) && (argv[1] != NULL)) {
		FILE *file = fopen(argv[1], "r");
		if (!file) {
            myPrint(error_message);
            exit(EXIT_FAILURE);
		} 
		
		// fgets reads each line as string
		while (fgets(cmd_buff, MAX_CMD_LENGTH+2, file) != NULL) {
            size_t len = strlen(cmd_buff);

            if (cmd_buff[len-1] != '\n') {
                myPrint(cmd_buff);
                continue;
            }
            else {
			    // ignore empty lines
                if (!is_empty(cmd_buff)) { 
			        myPrint(cmd_buff);
                
				    if (strchr(cmd_buff, ';') != NULL) {
					    sin_cmd = parse_command_to_node(cmd_buff, delim);
					    run_multiple_command(sin_cmd, args, delim_m, delim_r);
				    }
				    else {
		                if (strchr(cmd_buff, '>') != NULL) {
                            // if there is only one '>' entered
                            char *s = cmd_buff;
                            int occur = count_occur(s);
                            char *trim_s = remove_whitespace(s);
                            if (*trim_s == '>' || occur > 1) {
                                myPrint(error_message);
                                continue;
                            }

			                args = parse_single_command(cmd_buff, delim_r);
                            // if file name points to null
                            if (args[1] == NULL) {
                                myPrint(error_message);
                                continue;
                            } 
                        
                            test_args = parse_single_command(args[1], delim_m);
                            if (test_args[1] != NULL) {
                                myPrint(error_message);
                                continue;
                            }

                            // check if command has build-in commands
                            if (strstr(args[0], "cd") != NULL || strstr(args[0], "pwd") != NULL
                                        || strstr(args[0], "exit") != NULL) {
                                myPrint(error_message);
                                continue;
                            }

                            // parse the command again
                            red_args = parse_single_command(args[0], delim_m);

                            red_file = remove_whitespace(args[1]);
                            size_t len = strlen(red_file);
                            if (len > 0 && red_file[len-1] == '\n')
                                red_file[len-1] = '\0';

                            execute_redirection(red_args, red_file);
                        }
                        else {
					        // parse arguments from each line
					        args = parse_single_command(cmd_buff, delim_m);
                    
					        // execute the command
					        run_shell(args);
                        }
				    }
                }
            }
            //counter++;
		}
		fclose(file);
	} 
	else if ((argc == 1) && (argv[1] == NULL)){ // interactive mode
	    while (1) {
	        myPrint("myshell> ");	

			// get command line 
	        pinput = fgets(cmd_buff, MAX_CMD_LENGTH+2, stdin);	
	        if (!pinput) {
				myPrint(error_message);
	            exit(EXIT_FAILURE);
	        }

            if (strlen(pinput) > MAX_CMD_LENGTH) {
                myPrint(error_message);
                int c;
                while ((c = fgetc(stdin)) != EOF && c != '\n');
            }
            else {
				// Check for different cases with delimiters
				if (strchr(pinput, ';') != NULL) {
					//printf("Multiple command\n");
				
					// get the linked list struct 
					// each node stores a single command
					sin_cmd = parse_command_to_node(pinput, delim);
					run_multiple_command(sin_cmd, args, delim_m, delim_r);
				
				}
				else {
		        	if (strchr(pinput, '>') != NULL) {
                    	char *s = pinput;
                    	int occur = count_occur(s);
                    	char *trim_s = remove_whitespace(s);
                    	if (*trim_s == '>' || occur > 1) {
                        	myPrint(error_message);
                        	continue;
                        }
                   	  

			            args = parse_single_command(pinput, delim_r);

                        // if file name points to null
                        if (args[1] == NULL) {
                            myPrint(error_message);
                            continue;
                        } 
                        // check if second argument has length one
                        test_args = parse_single_command(args[1], delim_m);
                        if (test_args[1] != NULL) {
                            myPrint(error_message);
                            continue;
                        }

                        // check if build-in commands exist
                        if (strstr(args[0], "cd") != NULL || strstr(args[0], "pwd") != NULL
                                || strstr(args[0], "exit") != NULL) {
                            myPrint(error_message);
                            continue;
                        }

                    // trim whitespace and remove newline in filename
                        red_file = remove_whitespace(args[1]);
                        size_t len = strlen(red_file);
                        if (len > 0 && red_file[len-1] == '\n')
                            red_file[len-1] = '\0';

                        red_args = parse_single_command(args[0], delim_m);

                        execute_redirection(red_args, red_file);
		            }
                    else {
    				    // return an array of arguments from command line
				        args = parse_single_command(pinput, delim_m);
			
				        // deal with empty command line
				        if (args[0] == NULL) continue;
		
				        // execute the command
				        run_shell(args);
                    }
			    }
            }
	    }
	}
	else { // command line arguments incorrect
		myPrint(error_message);
		exit(EXIT_FAILURE);
	}

    return 0;
}

/***************************
* Function implementations *
****************************/
void myPrint(char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

void create_node(node_t **head, char *token) {
	node_t *curr;
	
	// if list is NULL, allocate mem for head node
	if (*head == NULL) {
		*head = (node_t*)malloc(sizeof(node_t));
		curr = *head;
	}
	else { // trasverse the list
		curr = *head;
		// traverse until NULL
		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = (node_t*)malloc(sizeof(node_t));
		curr = curr->next;
	}
	// end with NULL
	curr->next = NULL;
	curr->token = strdup(token);
}

void clear_node(node_t *node) {
	node_t *tmp;
	while (node != NULL) {
		tmp = node;
		node = node->next;
		free(tmp);
	}
}

node_t *parse_command_to_node(char *cmd, char *delim) {
	char *token;
	node_t *tokens = NULL;
	
	token = strtok(cmd, delim);
	while (token != NULL) {
		create_node(&tokens, token);
		token = strtok(NULL, delim);
	}
	return tokens;
}


int node_length(node_t *node) {
	node_t *curr = node;
	int cnt = 0;
	
	while (curr != NULL) {
		cnt++;
		curr = curr->next;
	}
	return cnt;
}

char **parse_single_command(char *cmd, char *delim) {
	node_t *node;
	int node_len;
	int pos = 0;
	char **result;
	
	// get linked-list of command arguments
	node = parse_command_to_node(cmd, delim);
	// get the size of the node
	node_len = node_length(node);
	
	// allocate mem for array
	result = malloc((node_len+1) * sizeof(char*));
	// fill array
	while ((node != NULL) && (pos <= node_len)) {
		result[pos] = node->token;
		node = node->next;
		pos++;
	} 
	// set the end with NULL 
	result[pos] = NULL;
	
	// free list structure
	clear_node(node);
	
	return result;
}

void buildin_cd(char **args) {
	// change dir to home path
	if (args[1] == NULL) {
		// success if 0 is returned
		if (chdir(getenv("HOME")) != 0) 
			myPrint(error_message);
	}
	else { // change to specified path in arg[1]
		// success if 0 is returned
		if (chdir(args[1]) != 0) 
			myPrint(error_message);
	}
}

void buildin_exit(char **args) {
	exit(EXIT_SUCCESS);
}

void execute_process(char **args) {
	pid_t c_pid;
	int status;
	
	c_pid = fork();
	
	if (c_pid == 0) {
		// child process
		if (execvp(args[0], args) == -1) {
			myPrint(error_message);
            exit(EXIT_FAILURE);
		}
	}
	else if (c_pid > 0) {
		// parent process
		do {
			waitpid(c_pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
    else {
        myPrint(error_message);
        exit(EXIT_FAILURE);
    }
}

void execute_redirection(char **args, char *file) {
    pid_t c_pid;
    int status;

    c_pid = fork();

    if (c_pid == 0) {
        int fd;
        if ((fd = open(file, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IROTH|S_IRGRP)) < 0) {
            myPrint(error_message);
            exit(0);
        }
        else {
		    if (dup2(fd, STDOUT_FILENO) == -1) 
                myPrint(error_message);
		
		    if (execvp(args[0], args) == -1) {
			    myPrint(error_message);
                exit(EXIT_FAILURE);
            }
        }
    }
    else if (c_pid > 0) {
        do {
            waitpid(c_pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    else {
        myPrint(error_message);
        exit(EXIT_FAILURE);
    }
}
    

void execute_command(char **args) {	
	// check for build-ins and execute accordingly
	if (strcmp(args[0], "cd") == 0) {
        // cannot have more than one arguments
        if (args[2] != NULL)
            myPrint(error_message);
        else
		    buildin_cd(args);
    }
	else if (strcmp(args[0], "exit") == 0) {
        // cannot have arguments follow it
        if (args[1] != NULL)
            myPrint(error_message);
        else 
		    buildin_exit(args);
    }
	// execute other commands
	else {
		//printf("Here in execute_command\n");
		execute_process(args);
		
	}
}

void run_shell(char **args) {
	// execute command for pwd
	char buff[MAX_CMD_LENGTH]; 
	if (strcmp(args[0], "pwd") == 0) {
        // if arguments followed by pwd, print error and stop
		if (args[1] != NULL) {
			myPrint(error_message);
		}
        else {
		    memset(buff, '\0', sizeof(buff));
		    if (getcwd(buff, sizeof(buff)) != NULL) {
		        myPrint(buff);
                myPrint("\n");
            }
        }
	}
    else  // execute other commands	
	    execute_command(args);

	// free allocated memories
	free(args);
}

void run_multiple_command(node_t *sin_cmd, char **args, char *delim_m, char *delim_r) {
	char *sin_cmdline;
    char *red_file;
    char **red_args;
    char **test_args;
	while (sin_cmd != NULL) {
		sin_cmdline = sin_cmd->token;
        if (!is_empty(sin_cmdline)) {
		    if (strchr(sin_cmdline, '>') != NULL) {
                char *s = sin_cmdline;
                int occur = count_occur(s);
                char *trim_s = remove_whitespace(s);
                if (*trim_s == '>' || occur > 1) {
                    myPrint(error_message);
                    continue;
                }

			    args = parse_single_command(sin_cmdline, delim_r);
                // if file name points to null
                if (args[1] == NULL) {
                    myPrint(error_message);
                    continue;
                } 

                // check if second argument has length one
                test_args = parse_single_command(args[1], delim_m);
                if (test_args[1] != NULL) {
                    myPrint(error_message);
                    continue;
                }
                
                // check if build-in commands exist
                if (strstr(args[0], "cd") != NULL || strstr(args[0], "pwd") != NULL
                        || strstr(args[0], "exit") != NULL) {
                    myPrint(error_message);
                    continue;
                }

                red_file = remove_whitespace(args[1]);
                size_t len = strlen(red_file);
                if (len > 0 && red_file[len-1] == '\n')
                    red_file[len-1] = '\0';
                red_args = parse_single_command(args[0], delim_m);

                execute_redirection(red_args, red_file);
		    }
		    else {
			    // pass this token to parse_singe_command function					
			    args = parse_single_command(sin_cmdline, delim_m);
			    if (args[0] == NULL) continue;
			    run_shell(args);
		    }
        }
		sin_cmd = sin_cmd->next;
	}
}

int is_empty(char *s) {
    while (*s != '\0') {
        if (!isspace(*s))
            return 0;
        s++;
    }
    return 1;
}

char *remove_whitespace(char *name) {
    char *end;
    while (isspace(*name)) name++;

    if (*name == '\0') 
        return name;

    end = name + strlen(name) - 1;
    while (end > name && isspace(*end)) end--;

    *(end+1) = '\0';
    return name;
}

int count_occur(char *cmd) {
    int count = 0;
    while (*cmd != '\0') {
        if (*cmd == '>')
            count++;
        cmd++;
    }
    return count;
}
