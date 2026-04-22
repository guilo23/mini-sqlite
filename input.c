#include "input.h"



InputBuffer* new_input_buffer(){
  InputBuffer* Input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  Input_buffer->buffer = NULL;
  Input_buffer->buffer_length = 0;
  Input_buffer->input_length =0;

  return  Input_buffer;
}

void print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void print_prompt(){
  printf("db > ");
  fflush(stdout);
}

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
void print_constants() {
  printf("ROW_SIZE: %d\n", ROW_SIZE);
  printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
  printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
  printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
  printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}
void indent(uint32_t level){
  for(uint32_t i = 0; i < level;i++){
    printf(" ");
  }
}
void print_tree(Pager* pager,uint32_t page_num,uint32_t indetation_level){
  void* node = get_page(pager,page_num);
  uint32_t num_keys,child;

  switch(get_node_type(node)){
    case (NODE_LEAF):
      num_keys = *leaf_node_num_cells(node);
      indent(indetation_level);
      printf("- leaf (size %d)\n",num_keys);
      for(uint32_t i = 0; i < num_keys;i++){
      indent(indetation_level + 1);
      printf("-%d\n",*leaf_node_key(node,i));
      }
      break;
    case (NODE_INTERNAL):
      num_keys = *internal_node_num_keys(node);
      indent(indetation_level);
      printf("- internal (size %d)\n",num_keys);
      for (uint32_t i = 0; i < num_keys;i++){
        child = *internal_node_child(node,i);
        print_tree(pager,child,indetation_level + 1);

        indent(indetation_level + 1);
        printf("- key %d\n", *internal_node_key(node,i));
      }
    child = *internal_node_right_child(node);
    print_tree(pager,child,indetation_level + 1);
    break;
    }
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer,Table* table){
  if(strcmp(input_buffer->buffer,".exit") == 0) {
      db_close(table);
      exit(EXIT_SUCCESS);
  } else if (strcmp(input_buffer->buffer,".btree") == 0) {
    printf("Tree:\n");
    print_tree(table->pager,0,0);
    return META_COMMAND_SUCCESS;
  } else if (strcmp(input_buffer->buffer,".constants") == 0) {
    printf("constants\n");
    print_constants();
    return META_COMMAND_SUCCESS;
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}
PrepareResult prepare_insert(InputBuffer* input_buffer,Statement* statement){
  statement->type = STATEMENT_INSERT;

  char* keyword = strtok(input_buffer->buffer," ");
  char* id_string = strtok(NULL," ");
  char* username = strtok(NULL," ");
  char* email = strtok(NULL," ");
  
  if (id_string == NULL || username == NULL || email == NULL){
    return PREPARE_SINTAXE_ERROR;
  }
  int id = atoi(id_string);
  if(id < 0){
    return PREPARE_NEGATIVE_ID;
  }
  if(strlen(username) > COLUMN_USERNAME_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }
  if (strlen(email) > COLUMN_EMAIL_SIZE){
    return PREPARE_STRING_TOO_LONG;
  }
  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username,username);
  strcpy(statement->row_to_insert.email,email);

  return PREPARE_SUCCESS;

}

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement){
  if(strncmp(input_buffer->buffer,"insert",6) == 0){
    return prepare_insert(input_buffer,statement);
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table){
  Row* row_to_insert = &(statement->row_to_insert);
  uint32_t key_to_insert = row_to_insert->id;

  Cursor* cursor = table_find(table,key_to_insert);

  void* node = get_page(table->pager, cursor->page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);

  if (cursor->cell_num < num_cells) {
  if (*leaf_node_key(node, cursor->cell_num) == key_to_insert) {
    free(cursor);
    return EXECUTE_DUPLICATE_KEY;
  }
}

if (cursor->cell_num > 0) {
  if (*leaf_node_key(node, cursor->cell_num - 1) == key_to_insert) {
    free(cursor);
    return EXECUTE_DUPLICATE_KEY;
  }
}

  leaf_node_insert(cursor,key_to_insert,row_to_insert);
  free(cursor);

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select (Statement* statement, Table* table){
  
  Cursor* cursor = table_start(table);

  Row row;
  while(!(cursor->end_of_table)){
    deserialize_row(cursor_value(cursor),&row);
    print_row(&row);
    cursor_advanced(cursor);
  }
  free(cursor);
  return EXECUTE_SUCCESS;
}
ExecuteResult execute_statement(Statement* statement,Table* table){
  switch (statement->type) {
    case(STATEMENT_INSERT):
      return execute_insert(statement,table); 
    case(STATEMENT_SELECT):
      return execute_select(statement,table);
  }
}
void run_main(char* filename){
   Table* table = db_open(filename);
  InputBuffer* Input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    read_input(Input_buffer);
    if (Input_buffer->buffer[0] == '.') {
      switch (do_meta_command(Input_buffer,table)) {
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
        case(PREPARE_NEGATIVE_ID):
        printf("id must be positive\n");
        continue;
        case(PREPARE_STRING_TOO_LONG):
        printf("String is to Long\n");
        continue;
        case(PREPARE_UNRECOGNIZED_STATEMENT):
          printf("Unrecognized keyword at start of '%s'.\n",Input_buffer->buffer);
          continue;
      }
     
      switch (execute_statement(&statement, table)) {
        case (EXECUTE_SUCCESS):
	        printf("Executed.\n");
	        break;
      case (EXECUTE_DUPLICATE_KEY):
          printf("Error: Duplicate key.\n");
          break;
	      case (EXECUTE_TABLE_FULL):
	        printf("Error: Table full.\n");
	        break;
     }
  }
}
