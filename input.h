#ifndef INPUT_H
#define INPUT_H
#include "pager.h"
#include "btree.h"
#include "table.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

typedef struct{
  char* buffer;
  size_t buffer_length;
  size_t input_length;
} InputBuffer;

typedef enum{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
}MetaCommandResult;

typedef enum {PREPARE_SUCCESS,PREPARE_UNRECOGNIZED_STATEMENT,PREPARE_NEGATIVE_ID,PREPARE_STRING_TOO_LONG,PREPARE_SINTAXE_ERROR} PrepareResult;

typedef enum { EXECUTE_SUCCESS,EXECUTE_DUPLICATE_KEY,EXECUTE_TABLE_FULL } ExecuteResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef struct {
  StatementType type;
  Row row_to_insert;
} Statement;

InputBuffer* new_input_buffer();

void print_row(Row* row);
void print_prompt();
void print_constants();
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer* input_buffer);
void indent(uint32_t level);
void print_tree(Pager* pager,uint32_t page_num,uint32_t indetation_level);

MetaCommandResult do_meta_command(InputBuffer* input_buffer,Table* table);

PrepareResult prepare_insert(InputBuffer* input_buffer,Statement* Statement);
PrepareResult prepare_statement(InputBuffer*,Statement* Statement);

ExecuteResult execute_insert(Statement*,Table* table);
ExecuteResult execute_select(Statement*,Table* table);
ExecuteResult execute_statement(Statement*,Table* table);

void run_main(char* filename);

#endif
