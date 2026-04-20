#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "table.h"
#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100

/*
 * =========================
 * ROW
 * =========================
 */

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE + 1];
  char email[COLUMN_EMAIL_SIZE + 1];
} Row;

/*
 * =========================
 * ROW LAYOUT
 * =========================
 */

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define ID_SIZE size_of_attribute(Row, id)
#define USERNAME_SIZE size_of_attribute(Row, username)
#define EMAIL_SIZE size_of_attribute(Row, email)

#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)

#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)

/*
 * =========================
 * NODE TYPES
 * =========================
 */

typedef enum {
  NODE_INTERNAL,
  NODE_LEAF
} NodeType;

typedef struct Table Table;
typedef struct Cursor Cursor;

/*
 * =========================
 * COMMON NODE HEADER
 * =========================
 */

#define NODE_TYPE_SIZE sizeof(uint8_t)
#define NODE_TYPE_OFFSET 0

#define IS_ROOT_SIZE sizeof(uint8_t)
#define IS_ROOT_OFFSET (NODE_TYPE_SIZE)

#define PARENT_POINTER_SIZE sizeof(uint32_t)
#define PARENT_POINTER_OFFSET (IS_ROOT_OFFSET + IS_ROOT_SIZE)

#define COMMON_NODE_HEADER_SIZE \
  (NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE)

/*
 * =========================
 * LEAF NODE
 * =========================
 */

#define LEAF_NODE_NUM_CELLS_SIZE sizeof(uint32_t)
#define LEAF_NODE_NUM_CELLS_OFFSET (COMMON_NODE_HEADER_SIZE)

#define LEAF_NODE_NEXT_LEAF_SIZE sizeof(uint32_t)
#define LEAF_NODE_NEXT_LEAF_OFFSET \
  (LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE)

#define LEAF_NODE_HEADER_SIZE \
  (COMMON_NODE_HEADER_SIZE + \
   LEAF_NODE_NUM_CELLS_SIZE + \
   LEAF_NODE_NEXT_LEAF_SIZE)

#define LEAF_NODE_KEY_SIZE sizeof(uint32_t)
#define LEAF_NODE_VALUE_SIZE ROW_SIZE

#define LEAF_NODE_CELL_SIZE \
  (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)

#define LEAF_NODE_SPACE_FOR_CELLS \
  (PAGE_SIZE - LEAF_NODE_HEADER_SIZE)

#define LEAF_NODE_MAX_CELLS \
  (LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)

#define LEAF_NODE_RIGHT_SPLIT_COUNT \
  ((LEAF_NODE_MAX_CELLS + 1) / 2)

#define LEAF_NODE_LEFT_SPLIT_COUNT \
  ((LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT)

/*
 * =========================
 * INTERNAL NODE
 * =========================
 */

#define INTERNAL_NODE_NUM_KEYS_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_NUM_KEYS_OFFSET (COMMON_NODE_HEADER_SIZE)

#define INTERNAL_NODE_RIGHT_CHILD_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_RIGHT_CHILD_OFFSET \
  (INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE)

#define INTERNAL_NODE_HEADER_SIZE \
  (COMMON_NODE_HEADER_SIZE + \
   INTERNAL_NODE_NUM_KEYS_SIZE + \
   INTERNAL_NODE_RIGHT_CHILD_SIZE)

#define INTERNAL_NODE_KEY_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_CHILD_SIZE sizeof(uint32_t)

#define INTERNAL_NODE_CELL_SIZE \
  (INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE)

#define INTERNAL_NODE_MAX_CELLS 3

#define INVALID_PAGE_NUM UINT32_MAX


//Temp functions
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination); 



uint32_t* leaf_node_num_cells(void* node);
uint32_t* leaf_node_next_leaf(void* node);
uint32_t* leaf_node_next_leaf(void* node);
uint32_t* internal_node_num_keys(void* node);
uint32_t* internal_node_right_child(void* node);
uint32_t* internal_node_cell(void* node,uint32_t cell_num);
uint32_t* internal_node_child(void* node,uint32_t child_num);
uint32_t* internal_node_key(void*node,uint32_t key_num);
uint32_t* node_parent(void* node);
uint32_t* leaf_node_key(void* node,uint32_t cell_num);
uint32_t get_node_max_type(Pager* pager,void* node);
uint32_t internal_node_find_child(void* node,uint32_t key);


bool is_node_root(void*node);

NodeType get_node_type(void* node);
void set_node_type(void* node,NodeType type);
void set_node_root(void* node, bool is_root);
void initialize_leaf_node(void* node);
void initialize_internal_node(void* node);
void internal_node_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num);
void create_new_root(Table* table,uint32_t right_child_page_num);
void update_internal_node_key(void* node,uint32_t old_key, uint32_t new_key);
void leaf_node_split(Cursor* cursor,uint32_t key,Row* value);
void internal_node_split_and_insert(Table* table,uint32_t parent_page_num,uint32_t child_page_num);
void leaf_node_insert(Cursor* cursor,uint32_t key,Row* value);
void* leaf_node_value(void* node, uint32_t cell_num);
void* leaf_node_cell(void* node, uint32_t cell_num);


Cursor* leaf_node_find(struct Table* table, uint32_t page_num, uint32_t key);
Cursor* internal_node_find(struct Table* table, uint32_t page_num, uint32_t key);

#endif
