#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024
#define RED "\033[0;31m"
#define RESET "\e[0m"

char *read_line();
char **parse_line(char *line, int *counter);


void loop(){
  char *line;
  char ** tokens;
  int counter;
  int status = 1;

  while (status){
    fprintf(stdout, ">: ");
    line = read_line();
    tokens = parse_line(line, &counter);

    printf("%d\n", counter);
    for (int i = 0; i < counter; i++){
      printf("%s\n", tokens[i]);
    }


    free(line);
    free(tokens);
  }
}

int main(){
  loop();
  return 0;
}

char *read_line() {
  int buffsize = 1024;
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
      buffsize += 1024;
      buffer = realloc(buffer, buffsize);

      if (!buffer) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **parse_line(char *line, int *counter){
  int buffer_size = BUF_SIZE;
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