#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "pager.h"
#include "btree.h"

typedef struct Table {
  Pager* pager;
  uint32_t root_page_num;
} Table;

typedef struct Cursor {
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;
} Cursor;

/* Protype */
Cursor* table_find(Table* table, uint32_t key);
Cursor* table_start(Table* table);
void cursor_advanced(Cursor* cursor);
void* cursor_value(Cursor* cursor);
Table* db_open(const char* filename);
void db_close(Table* table);

#endif
