/*
  HOU - Header Only Utilities, header only library with many useful structs and functions
 */

#ifndef HOU_H__
#define HOU_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // really for memcpy (yes c is so STUPID that memory functions are in string.h)

#ifdef HOU_IMPLEMENTATION
#define HOU_FUNC_SPEC static
#else
#define HOU_FUNC_SPEC 
#endif

/**
   Internal tmp variables of hou
 */

#ifdef HOU_TMP_VARS_DEFINED_IN_C_EXTERN
#define HOU_TMP_SPEC extern
#else
#define HOU_TMP_SPEC static
#endif

HOU_TMP_SPEC bool __hou_stored_ptr_lock = false;
HOU_TMP_SPEC void* __hou_stored_ptr = NULL;

/**
   Fail
 */

#define fail_if(cond, reason) \
  if(!(cond)) {								\
   printf("Condition in %s:%d failed, reason: %s\n", __FILE__, __LINE__, reason); \
    exit(-1); }

/**
   Dynamic Arrays
 */

#ifndef HOU_DA_CHUNK_SIZE
#define HOU_DA_CHUNK_SIZE 128
#endif

typedef struct {
  void* arr;
  size_t arrAllocated;
  size_t arrUsed;
} da_general; 

#define generate_da_type(type) typedef struct {				\
    type* arr;								\
    size_t arrAllocated;						\
    size_t arrUsed;							\
  } da_##type;								\
									\
  da_##type __create_da_##type() {					\
    da_##type* p = create_da_ptr(type);					\
    da_##type v = *p;							\
    free(p);								\
    return v;								\
  }									

HOU_FUNC_SPEC da_general create_da_general(size_t elem_size);
HOU_FUNC_SPEC da_general *create_allocated_da_general(size_t elem_size);
HOU_FUNC_SPEC void append_to_da_general(da_general* da, size_t element_sizeof, const void* element);
HOU_FUNC_SPEC void remove_last_from_da_general(da_general* da, size_t element_sizeof);
HOU_FUNC_SPEC void insert_da_general(da_general* da, size_t element_sizeof, const void* element, size_t index);
HOU_FUNC_SPEC void remove_from_index_da_general(da_general* da, size_t element_sizeof, size_t index);

#define create_da_ptr(type) (da_##type*)create_allocated_da_general(sizeof(type)); 

#define create_da(type) __create_da_##type()

#define append_to_da(da, element)					\
  fail_if(sizeof(element) == sizeof(*(da->arr)), "Your element isn't type of element in array"); \
  while(__hou_stored_ptr_lock) {}					\
  __hou_stored_ptr_lock = true;						\
  __hou_stored_ptr = malloc(sizeof(*(da->arr)));			\
  fail_if(__hou_stored_ptr != NULL, "Allocation Error");		\
  *(typeof(element)*)__hou_stored_ptr = element;			\
  append_to_da_general((da_general* )da, sizeof(*(da->arr)), __hou_stored_ptr); \
  free(__hou_stored_ptr);						\
  __hou_stored_ptr_lock = false;

// b- but casting pointers is undefined behavior
// HOW THE (INSERT SWEAR HERE) THIS IS A (INSERT SWEAR HERE) UNDEFINED BEHAVIOR
// FOR SOMEONE THAT THINKED THAT WAY I HAVE A MESSAGE: IT'S NORMAL
// POINTER POINTS ON MEMORY, CASTING IT TO ANOTHER TYPEDEF IS NORMAL
// FOR PEOPLE THAT THINK THAT THING THIS CAN BREAK:
// IT SHOULDN'T: THERE IS THE SAME STRUCTURE OF STRUCT
#define remove_last_from_da(da) \
  remove_last_from_da_general((da_general* )da, sizeof(*da->arr));

#define insert_da(da, element, index)					\
  fail_if(sizeof(element) == sizeof(*(da->arr)), "Your element isn't type of element in array"); \
  while(__hou_stored_ptr_lock) {}					\
  __hou_stored_ptr_lock = true;						\
  __hou_stored_ptr = malloc(sizeof(*(da->arr)));			\
  fail_if(__hou_stored_ptr != NULL, "Allocation Error");		\
  *(typeof(element)*)__hou_stored_ptr = element;			\
  insert_da_general((da_general* )da, sizeof(*(da->arr)), __hou_stored_ptr, index); \
  free(__hou_stored_ptr);						\
  __hou_stored_ptr_lock = false;

#define remove_from_index_da(da, index)					\
  remove_from_index_da_general((da_general* )da, sizeof(*da->arr), index);

#define da_foreach(da, function_name) \
  for(int i = 0; i < da.arrUsed; i++) function_name(i, &(da.arr[i]))

#ifdef HOU_IMPLEMENTATION

static da_general create_da_general(size_t elem_size) {
  da_general da;
  da.arr = malloc(elem_size * HOU_DA_CHUNK_SIZE);
  fail_if(da.arr != NULL, "Allocation Error"); // If malloc results NULL it should throw error of allocation
  da.arrAllocated = HOU_DA_CHUNK_SIZE;
  da.arrUsed = 0;  
  return da;
}

static da_general* create_allocated_da_general(size_t elem_size) {
  da_general *da = malloc(sizeof(da_general));
  fail_if(da != NULL, "Allocation Error");

  *da = create_da_general(elem_size);
  return da;
}

static void append_to_da_general(da_general* da, size_t element_sizeof, const void* element) {
  da->arrUsed++;
  if(da->arrUsed > da->arrAllocated) {
    void* realloced = realloc(da->arr, (da->arrAllocated + HOU_DA_CHUNK_SIZE) * element_sizeof);
    if(realloced == NULL) {
      free(da->arr);
      fail_if(true, "Allocaton Error");
    }
    da->arr == realloced;
  }

  // memcpy(da->arr + ((da->arrUsed - 1) * HOU_DA_CHUNK_SIZE), element, element_sizeof);
  void* startingPoint = da->arr + ((da->arrUsed - 1) * element_sizeof);
  memcpy(startingPoint, element, element_sizeof);
}

static void remove_last_from_da_general(da_general* da, size_t element_sizeof) {
  da->arrUsed--;
  if(da->arrUsed < da->arrAllocated - HOU_DA_CHUNK_SIZE) {
    void* realloced = realloc(da->arr, (da->arrAllocated - HOU_DA_CHUNK_SIZE) * element_sizeof);
    if(realloced == NULL) {
      free(da->arr);
      fail_if(true, "Allocaton Error");
    }
    da->arr == realloced;
  }
}

static void insert_da_general(da_general* da, size_t element_sizeof, const void* element, size_t index) {
  da->arrUsed++;
  if(da->arrUsed > da->arrAllocated) {
    void * realloced = realloc(da->arr, (da->arrAllocated + HOU_DA_CHUNK_SIZE) * element_sizeof);
    if(realloced == NULL) {
      free(da->arr);
      fail_if(true, "Allocaton Error");
    }
    da->arr == realloced;
  }

  void* index_in_arr = da->arr + index * element_sizeof;
  
  // Moving data forward
  memmove(index_in_arr + element_sizeof, index_in_arr, (da->arrUsed - index) * element_sizeof);

  // Add value
  memcpy(index_in_arr, element, element_sizeof);
}

static void remove_from_index_da_general(da_general* da, size_t element_sizeof, size_t index) {
  da->arrUsed--;

  void* index_in_arr = da->arr + index * element_sizeof;
  
  memmove(index_in_arr - element_sizeof, index_in_arr, (da->arrUsed - index + 1) * element_sizeof);
  
  if(da->arrUsed < da->arrAllocated - HOU_DA_CHUNK_SIZE) {
    void* realloced = realloc(da->arr, (da->arrAllocated - HOU_DA_CHUNK_SIZE) * element_sizeof);
    if(realloced == NULL) {
      free(da->arr);
      fail_if(true, "Allocaton Error");
    }
    da->arr == realloced;
  }
}

#endif

#endif
