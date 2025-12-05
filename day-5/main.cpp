#include "vector.h"
#include "readfile.h"
#include "math.h"


typedef struct {
  long long lower;
  long long upper;
} Range;


int isinRange(long long x, Range range) {
  return x >= range.lower && x <= range.upper;
}

void printRange(Range range) {
  printf("[%lld, %lld]\n", range.lower, range.upper);
}


void freeRange(const Range *range) { return; }
CREATE_VECTOR_ASSETS(Range, RangeVector, printRange, 0, freeRange);


int sign(long long x) {
  return x < 0 ? -1 : x > 0 ? 1: 0;
}

int compareRanges(const void *x, const void *y) {
  return sign(((Range *)x)->lower - ((Range *)y)->lower);
}

void sortRangeVector(RangeVector *ranges) {
  qsort(ranges->values, ranges->length, sizeof(Range), compareRanges);
}


int isinRangeVector(long long x, RangeVector ranges) {
  // Herw we assume the ranges vector has been sorted
  for (int i = 0; i < ranges.length; i++) {
    if (isinRange(x, ranges.values[i])) return 1;
    else if (x < ranges.values[i].upper) return 0;
  }

  return 0;
}


typedef struct {
  RangeVector ID_Ranges;
  LongLongIntVector IDs;
} Inventory;


void printInventory(Inventory inventory) {
  printf("INVENTORY\nValid IDs:\n");
  printRangeVector(inventory.ID_Ranges);
  printf("IDs:\n");
  printLongLongIntVector(inventory.IDs); 
}


Inventory initInventory() {
  Inventory inventory;
  inventory.IDs = initLongLongIntVector();
  inventory.ID_Ranges = initRangeVector();

  return inventory;
}


unsigned long long countValidIDs(Inventory inventory) {
  RangeVector ranges = inventory.ID_Ranges;

  unsigned long long count = 0;
  Range bigrange = ranges.values[0];

  for (int i = 1; i < ranges.length; i++) {
    const Range range = ranges.values[i];
    const long long lower = range.lower;
    const long long upper = range.upper;

    if (lower <= bigrange.upper) { // overlapping
      bigrange.upper = upper > bigrange.upper ? upper : bigrange.upper;

    } else {
      count += bigrange.upper - bigrange.lower + 1;
      bigrange.lower = lower;
      bigrange.upper = upper;
    }
  }
  count += bigrange.upper - bigrange.lower + 1;

  return count;
}


void freeInventory(Inventory *inventory) {
  freeLongLongIntVector(&inventory->IDs);
  freeRangeVector(&inventory->ID_Ranges);
}


void parseLine(Inventory* inventory, String line) {
  long long numbers[2] = {0, 0};
  long long *idptr = numbers;

  int isID_Range = 0;

  for (int i = 0; i < line.length; i++) {
    const char c = line.values[i];

    if (c == '-') {
      idptr++;
      isID_Range = 1;
      continue;
    }

    *idptr = (*idptr) * 10 + c - '0';
  }

  if (isID_Range) {
    Range range;
    range.lower = numbers[0];
    range.upper = numbers[1];
    appendRangeVector(&inventory->ID_Ranges, range);

  } else {
    appendLongLongIntVector(&inventory->IDs, numbers[0]);
  }
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector input = readfile(argv[1], '\n');
  Inventory inventory = initInventory();
  printStringVector(input);

  int counter_t1 = 0;
  unsigned long long counter_t2 = 0;

  for (int i = 0; i < input.length; i++) {
    String line = input.values[i];
    parseLine(&inventory, line);
  }

  sortRangeVector(&inventory.ID_Ranges);
  printInventory(inventory);


  for (int i = 0; i < inventory.IDs.length; i++) {
    long long id = inventory.IDs.values[i];
    counter_t1+=isinRangeVector(id, inventory.ID_Ranges);
  }

  printf("Answer task 1: %d\n", counter_t1);
  printf("Answer task 2: %llu\n", countValidIDs(inventory));
  freeStringVector(&input);
  freeInventory(&inventory);
  
  return 0;
}
