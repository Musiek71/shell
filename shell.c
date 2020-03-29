#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUFF_SIZE 128
#define RED "\e[91m"
#define RESET "\e[0m"

//TODO make more use of chdir (internal cmd)
//TODO internal commands
//TODO cool colours

char *read_line();
char **parse_line(char *line, int *counter);
int shell_execute(char **args, int arg_count);
int shell_exit(char **args);
void print_tokens(char **tokens, int counter);
int check_internal_commands(char **args, int arg_count);
void print_help();



void loop(){
  char *line;
  char ** tokens;
  int counter;
  int status = 1;
  char tmp[128];


  do {
    printf("%s%s>%s ", RED, getcwd(tmp, 128), RESET);
    line = read_line();
    tokens = parse_line(line, &counter);

    //print_tokens(tokens, counter);
    if (counter < 1) continue;

    status = shell_execute(tokens, counter);

    free(line);
    free(tokens);
  } while (status);
}

int main(){
  loop();
  return 0;
}

char *read_line() {
  int buffsize = BUFF_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "Allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();
    if (c == EOF || c == '\n' || c == '\0' || c == '\r') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    if (position >= buffsize) {
      buffsize *= 2;
      buffer = realloc(buffer, buffsize);

      if (!buffer) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **parse_line(char *line, int *counter){
  int buffer_size = BUFF_SIZE;
  char ** tokens = malloc(buffer_size * sizeof(char *));
  char *token;

  *counter = 0;

  if (!tokens){
    fprintf(stderr, "Allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, " ");

  while (token != NULL){
    //printf("%s\n", token);
    tokens[*counter] = token;
    (*counter)++;

    if (*counter >= buffer_size) {
      buffer_size *= 2;
      tokens = realloc(tokens, buffer_size * sizeof(char * ));

      if (!tokens[*counter]) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, " ");
  }

  return tokens;

}

int shell_execute(char **args, int arg_count){
  pid_t cpid;
  int process_status;
  int flag;

  flag = check_internal_commands(args, arg_count);
  switch (flag){
    case (-1): return 0;
    case (1): return 1;
  }


  cpid = fork();

  if (cpid == 0) {
    if (execvp(args[0], args) < 0)
      printf("command not found: \"%s\"\n", args[0]);
    exit(EXIT_FAILURE);

  } else if (cpid < 0)
    printf(RED "Error forking"
      RESET "\n");
  else {
    waitpid(cpid, &process_status, WUNTRACED);
  }
  return 1;
}

int shell_exit(char **args){
  return -1;
}

void print_tokens(char **tokens, int counter){
    printf("command count:%d\n", counter);
    for (int i = 0; i < counter; i++){
      printf("%s\n", tokens[i]);
    }
}

int check_internal_commands(char **args, int arg_count){
  char tmp[128];

  if (strcmp(args[0], "exit") == 0) {
    return shell_exit(args);
  }
  else if (strcmp(args[0], "help") == 0){
    print_help();
    return 1;
  } 
  else if (strcmp(args[0], "cwd") == 0){
    printf("%s\n", getcwd(tmp, 128));
    return 1;
  } 
  else if (strcmp(args[0], "cd") == 0){
    if (chdir(args[1]) != 0){
      fprintf(stderr, "Failed to chdir to \"%s\"\n", args[1]);
      //TODO include errno
    }
    printf("%s\n", getcwd(tmp, 128));
    return 1;
  }
  else if (strcmp(args[0], "xD") == 0){
    printf("            /$$$$$$$ \n");
    printf("          | $$__  $$\n");
    printf(" /$$   /$$| $$  \\ $$\n");
    printf("|  $$ /$$/| $$  | $$\n");
    printf(" \\  $$$$/ | $$  | $$\n");
    printf("  >$$  $$ | $$  | $$\n");
    printf(" /$$/\\  $$| $$$$$$$/\n");
    printf("|__/  \\__/|_______/ \n");  
    return 1;
  }
  else return 0;
}

void print_help(){
  printf("Available commands:\n");
  printf("cwd - shows your current working dir\n");
  printf("cd [arg] - changes your working dir\n");
  printf("exit - exits\n");
  printf("xD - check it out!\n");
  printf("All external commands also do work\n");
}