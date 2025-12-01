#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_CAPACITY 8

#define CREATE_VECTOR_ASSETS(type, name, printFunc, freeScalars) \
                                                                 \
typedef struct {                                                 \
  type* values;                                                  \
  long int length;                                               \
  long int capacity;                                             \
} name;                                                          \
                                                                 \
name init##name() {                                              \
  name vec;                                                      \
                                                                 \
  vec.values   = (type*)malloc(DEFAULT_CAPACITY * sizeof(type)); \
  vec.capacity = DEFAULT_CAPACITY;                               \
  vec.length   = 0;                                              \
                                                                 \
  return vec;                                                    \
}                                                                \
                                                                 \
                                                                 \
void append##name(name* vec, const type value) {                 \
  if (vec->length >= vec->capacity)                              \
    vec->values = (type*)realloc(vec->values,                    \
        (vec->capacity*=2) * sizeof(type));                      \
                                                                 \
  vec->values[vec->length++] = value;                            \
}                                                                \
                                                                 \
                                                                 \
void free##name(name* vec) {                                     \
  if (freeScalars) {                                             \
    for (int i = 0; i < vec->length; i++)                        \
      free##type(&vec->values[i]);                               \
  }                                                              \
  free(vec->values);                                             \
  vec->capacity = 0;                                             \
  vec->length   = 0;                                             \
}                                                                \
                                                                 \
void print##name(name vec) {                                     \
  for (int i = 0; i < vec.length; i++)                           \
    printFunc(vec.values[i]);                                    \
  printf("\n");                                                  \
}                                                                \


void freeint(const int* x) { return; } // just stops compile errors, its never called
void freechar(const char* x) { return; } // just stops compile errors, its never called
                                           //
void printInt(const int x) { printf("%d\n", x); }
void printChar(const char x) { printf("%c\n", x); }
void printStringChar(const char x) { printf("%c", x); }

CREATE_VECTOR_ASSETS(int, IntVector, printInt, 0);
CREATE_VECTOR_ASSETS(char, CharVector, printChar, 0);
CREATE_VECTOR_ASSETS(char, String, printStringChar, 0);
CREATE_VECTOR_ASSETS(String, StringVector, printString, 1);
