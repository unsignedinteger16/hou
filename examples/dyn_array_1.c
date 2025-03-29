#include <stdio.h>

#define HOU_IMPLEMENTATION
#include "../hou.h"

generate_da_type(int);

void main() {
  da_int da = create_da(int);
  
  append_to_da((&da), 2); // 2
  append_to_da((&da), 5); // 2 5
  append_to_da((&da), 3); // 2 5 3
  insert_da((&da), 1, 0); // 1 2 5 3
  remove_last_from_da((&da)); // 1 2 5
  append_to_da((&da), 7); // 1 2 5 7
  insert_da((&da), 3, 3); // 1 2 5 3 7
  remove_from_index_da((&da), 3) // 1 2 3 7
  

  for (int i = 0; i < da.arrUsed; i++) {
    printf("%dth element is: %d\n", i, da.arr[i]);
  }
}
