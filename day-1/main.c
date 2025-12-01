#include "vector.h"




int main() {
  String vec = initString();

  for (int i = 0; i < 28; i++)
    appendString(&vec, i + 'A');
  printString(vec);
  freeString(&vec);
  
  return 0;
}
