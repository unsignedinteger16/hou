#include <stdio.h>
#include <stddef.h>

#define HOU_IMPLEMENTATION
#include "../hou.h"

generate_da_type(int);

void foreach_print_elements(size_t index, int *element) {
  printf("element %d is %d\n", index, *element);
}

void foreach_add_one(size_t index, int *element) {
  (*element)++;
}

void main() {
  da_int da = create_da(int);
  
  append_to_da((&da), 2); // 2
  append_to_da((&da), 5); // 2 5
  append_to_da((&da), 3); // 2 5 3
  insert_da((&da), 1, 0); // 1 2 5 3
  remove_last_from_da((&da)); // 1 2 5
  append_to_da((&da), 7); // 1 2 5 7
  insert_da((&da), 3, 3); // 1 2 5 3 7
  remove_from_index_da((&da), 3); // 1 2 3 7
  
  da_foreach(da, foreach_print_elements);
  da_foreach(da, foreach_add_one);
  da_foreach(da, foreach_print_elements);
}
