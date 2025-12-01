#inlucde <stlib.h>

#define DEFAULT_CAPACITY 8

typedef struct {
  const int* values;
  const long int length;
  const long int capacity;
} IntVector;


IntVector initIntVector() {
  IntVector vec;

  vec.values   = (int*)malloc(DEFAULT_CAPACITY * sizeof(int));
  vec.capacity = DEFAULT_CAPACITY;
  vec.length   = 0;

  return vec;
}


void appendIntVector(IntVector* vec, const int value) {
  if (vec->length >= vec->capacity)
    vec.values = (int*)realloc(vec->values, vec.capacity*=2);

  vec->values[vec.length++] = value;
}


void freeIntVector(IntVector* vec) {
  free(vec->values);
  vec->capacity = 0;
  vec->size     = 0;
}
