#include "pager.h"
#include "table.h"
#include "btree.h"
#include "input.h"

int main(int argc,char* argv[]){
  if(argc < 2){
      printf("Must supply a database filename.\n");  
    exit(EXIT_FAILURE);
    }
  char* filename = argv[1];
  run_main(filename);
  return EXIT_SUCCESS;
}
