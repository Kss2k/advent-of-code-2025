#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_CAPACITY 8

#define CREATE_VECTOR_ASSETS(type, name, printFunc, freeScalars, freeFunc) \
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
      freeFunc(&vec->values[i]);                                 \
  }                                                              \
  free(vec->values);                                             \
  vec->capacity = 0;                                             \
  vec->length   = 0;                                             \
}                                                                \
                                                                 \
void print##name(name vec) {                                     \
  for (int i = 0; i < vec.length; i++)                           \
    printFunc(vec.values[i]);                                    \
}                                                                \
                                                                 \
                                                                 \
name copy##name(name vec) {                                      \
  type* copiedvals;                                              \
  memcpy(copiedvals, vec.values, vec.capacity * sizeof(type));   \
  vec.values = copiedvals;                                       \
  return vec;                                                    \
}                                                                \


void freeint(const int* x) { return; } // just stops compile errors, its never called
void freechar(const char* x) { return; } // just stops compile errors, its never called
void freellint(const long long* x) { return; }

void printInt(const int x) { printf("%d\n", x); }
void printLongLongInt(const int x) { printf("%lld\n", x); }
void printChar(const char x) { printf("%c\n", x); }
void printStringChar(const char x) { printf("%c", x); }

CREATE_VECTOR_ASSETS(int, IntVector, printInt, 0, freeint);
CREATE_VECTOR_ASSETS(long long, LongLongIntVector, printLongLongInt, 0, freellint);
CREATE_VECTOR_ASSETS(char, CharVector, printChar, 0, freechar);
CREATE_VECTOR_ASSETS(char, String, printStringChar, 0, freechar);

void printStringNewline(const String x) { printString(x); printf(" [%d]\n", x.length); }
void printIntArraySpace(const IntVector x) { for (int i = 0; i < x.length; i++) printf("%d ", x.values[i]); printf("\n"); }

CREATE_VECTOR_ASSETS(String, StringVector, printStringNewline, 1, freeString);
CREATE_VECTOR_ASSETS(IntVector, IntArray, printIntArraySpace, 1, freeIntVector);

#endif
