#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUFF_SIZE 128
#define RED "\e[91m"
#define RESET "\e[0m"

//TODO make more use of chdir
//TODO internal commands
//TODO cool colours
//TODO command history
//TODO arrowkey command history
//TODO CLEANING
//TODO get rid of that stoopid ISO warning
//TODO get another BUFF_SIZE definition to distinguish the line buffer

char *read_line();
char **parse_line(char *line, int *counter);
int shell_execute(char **args, int arg_count);
int shell_exit(char **args);
void print_tokens(char **tokens, int counter);
int check_internal_commands(char **args, int arg_count);
void print_help();
void save_commands(char **array, int *cmd_count, char *line);
void realloc_2d_arr(char ***array_ptr, int *cmd_buff_size_ptr);
void print_history(char **array, int cmd_count);


void loop(){
  char *line;
  char **saved_commands = malloc(BUFF_SIZE * sizeof(char *));
  char **tokens;
  char ***saved_commands_ptr = &saved_commands;
  int counter;
  int cmd_count = 0;
  int *cmd_count_ptr = &cmd_count;
  int cmd_buff_size = BUFF_SIZE;
  int *cmd_buff_size_ptr = &cmd_buff_size;
  int status = 1;
  int saved_commands_index;
  char tmp[128];



  do{
    printf("%s%s>%s ", RED, getcwd(tmp, 128), RESET);
    //printf("%s> ", getcwd(tmp, 128));
    switch (status){
      case (3):
        line = saved_commands[saved_commands_index];
        status = 1;
        break;
      default:
        line = read_line();
    }

    //DEBUGGING
    //printf("buffsize:%d\n", cmd_buff_size);

    if (cmd_count > cmd_buff_size){
      realloc_2d_arr(saved_commands_ptr, cmd_buff_size_ptr);
    }
    save_commands(saved_commands, cmd_count_ptr, line);


    tokens = parse_line(line, &counter);
    print_tokens(tokens,counter);

    if (counter < 1)
      continue;

    status = shell_execute(tokens, counter);

    switch (status){
      case (2):
        print_history(saved_commands, cmd_count);
        status = 1;
        break;
      case (3):
        sscanf(tokens[2], "%d", &saved_commands_index);
        status = 3;
        break;
    }

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

  if (!buffer){
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
    case (-1): return 0; //exit
    case (1): return 1; //internal command executed
    case (2): return 2; //print history
    case (3): return 3; //use command from history
  }


  cpid = fork();

  if (cpid == 0) {
    if (execvp(args[0], args) < 0)
      printf("command not found: \"%s\"\n", args[0]);
    exit(EXIT_FAILURE);

  } else if (cpid < 0)
    printf("Error forking\n");
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

  if (strcmp(args[0], "exit") == 0) {
    return shell_exit(args);
  }
  if (strcmp(args[0], "history") == 0) {
    if (arg_count >1) {
      if (strcmp(args[1], "-i") == 0 && arg_count == 3)
        return 3;
    }
    else 
      return 2;
    
  }
  else if (strcmp(args[0], "help") == 0){
    print_help();
    return 1;
  } 
  else if (strcmp(args[0], "cd") == 0){
    if (chdir(args[1]) != 0){
      fprintf(stderr, "Failed to chdir to \"%s\"\n", args[1]);
      //TODO include errno
    }
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
  printf("cd [arg] - changes your working dir\n");
  printf("history - prints your command history\n");
  printf("xD - check it out!\n");
  printf("exit - exits\n");
}

void save_commands(char **array, int *cmd_count, char *line){
  if(strcmp(line, "") != 0 && strcmp(line, "history") != 0 && line[0] != '\0'){
    *cmd_count += 1;

    array[*cmd_count - 1] = malloc(BUFF_SIZE * sizeof(char));
    if (!array) {
      fprintf(stderr, "Allocation error\n");
      exit(EXIT_FAILURE);
    }

    strcpy(array[*cmd_count - 1], line);
  }
}

void realloc_2d_arr(char ***array_ptr, int *cmd_buff_size_ptr){
  *cmd_buff_size_ptr *= 2;
  *array_ptr = realloc(*array_ptr, *cmd_buff_size_ptr * sizeof(char *));
  if (!(*array_ptr)) {
    fprintf(stderr, "Allocation error\n");
    exit(EXIT_FAILURE);
  }
}

void print_history(char **array, int cmd_count){
  printf("cmd_count:%d\n", cmd_count);
  printf("ADRES      - indeks - slowo\n");
  for (int i = 0; i < cmd_count; i++)
    printf("%d - %d     - %s\n", &array[i], i, array[i]);
}