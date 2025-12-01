#include "vector.h"




int main() {
  IntVector vec = initIntVector();

  freeIntVector(&vec);

  return 0;
}
