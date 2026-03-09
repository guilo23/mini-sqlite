#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  char* buffer;
  size_t buffer_length;
  size_t input_length;
} InputBuffer;

typedef enum{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
}MetaCommandResult;

typedef enum {PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT} PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef struct {
  StatementType type;
} Statement;

InputBuffer* new_input_buffer(){
  InputBuffer* Input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  Input_buffer->buffer = NULL;
  Input_buffer->buffer_length = 0;
  Input_buffer->input_length =0;

  return  Input_buffer;
}

void print_prompt(){printf("db >");}

void read_input(InputBuffer* Input_buffer){
  ssize_t bytes_read = getline(&(Input_buffer->buffer),
                               &(Input_buffer->buffer_length),stdin);
  if(bytes_read <= 0){
    printf("Error reading input.\n");
    exit(EXIT_FAILURE);
  }
  Input_buffer->input_length = bytes_read - 1;
  Input_buffer->buffer[bytes_read - 1] = 0;
}
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer){
  if(strcmp(input_buffer->buffer,".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement){
  if(strncmp(input_buffer->buffer,"insert",6) == 0){
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer,"select") == 0){
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement){
  switch (statement->type) {
    case(STATEMENT_INSERT):
      printf("This is where we would do an insert.\n");
      break;
    case(STATEMENT_SELECT):
      printf("This is where we would do a select.\n");
      break;
  }
}

int main(int argc,char* argv[]){
  InputBuffer* Input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    read_input(Input_buffer);
    if (Input_buffer->buffer[0] == '.') {
      switch (do_meta_command(Input_buffer)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'\n",Input_buffer->buffer);
          continue;
      }
    }
      Statement statement;
      switch (prepare_statement(Input_buffer,&statement)) {
        case(PREPARE_SUCCESS):
          break;
        case(PREPARE_UNRECOGNIZED_STATEMENT):
          printf("Unrecognized keyword at start of '%s'.\n",Input_buffer->buffer);
          continue;
      }
      execute_statement(&statement);
      printf("Executed.\n");
  }
}
