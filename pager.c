#include "pager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/*
 * Open database file and initialize pager
 */

Pager* pager_open(const char* filename){
  int fd = open(filename,
                O_RDWR |     // READ/WRITE mode
                    O_CREAT, // CREATE file if it doesnt exist
                S_IWUSR |    // USER Write permission
                S_IRUSR      // USER read permission
                );

  if (fd == -1){
    printf("Unable to open file\n");
    exit(EXIT_FAILURE);
  }

  off_t file_length = lseek(fd,0,SEEK_END);

  Pager* pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;
  pager->num_pages = (file_length / PAGE_SIZE);
  
  if(file_length % PAGE_SIZE != 0){
    printf("Db file is not a whole number of pages. corrupt file.\n ");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++){
    pager->pages[i] = NULL;
  }
  return pager;
}
/*
 * Get page from cache or disk
 */
void* get_page(Pager* pager,uint32_t page_num){
  if(page_num > TABLE_MAX_PAGES){
    printf("Tried to fetch pahe number out of bounds.d %d > %d\n",page_num,
           TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }
  if (pager->pages[page_num] == NULL){
    // cache miss. allocate memory and load from file.
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;

    //might save a partial page at the end of the file
    if(pager->file_length % PAGE_SIZE){
      num_pages += 1;
    }
    if (page_num < num_pages ){
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor,page,PAGE_SIZE);
      if(bytes_read == -1){
        printf("Error reading file: %d\n",errno);
        exit(EXIT_FAILURE);
      }
    }
    pager->pages[page_num] = page;

    if(page_num >= pager->num_pages){
      pager->num_pages = page_num + 1;
    }
  }
  return pager->pages[page_num];
}
/*
 * Write page to disk
 */
void pager_flush(Pager* pager, uint32_t page_num){
  if(pager->pages[page_num] == NULL){
    printf("tried to flush null page\n");
    exit(EXIT_FAILURE);
  }
  off_t offset = lseek(pager->file_descriptor,page_num * PAGE_SIZE,SEEK_SET);

  if(offset == -1){
    printf("Error seeking: %d\n",errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written =
      write(pager->file_descriptor,pager->pages[page_num],PAGE_SIZE);

  if(bytes_written == -1){
    printf("Error writing: %d\n",errno);
    exit(EXIT_FAILURE);
  } 
}
/*
 * Allocate new page
 */
uint32_t get_unused_page_num(Pager* pager){
  return pager->num_pages;
}

