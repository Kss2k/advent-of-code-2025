#include "vector.h"
#include "readfile.h"
#include "math.h"


int getJoltageT1(String battery) {
  int b0 = 0;
  int b1 = 0;

  int i0;
  for (int i = 0; i < battery.length - 1L; i++) { // last value not available
    const int x = battery.values[i] - '0';

    if (x <= b0) 
      continue;

    b0 = x;
    i0 = i;
  }

  for (int i = i0+1L; i < battery.length; i++) {
    const int x = battery.values[i] - '0';

    if (x <= b1) 
      continue;

    b1 = x; 
  }

  return b0 * 10 + b1;
}


unsigned long long getJoltageT2(String battery, const int n) {
  unsigned long long joltage = 0L;

  int offset = 0L;
  for (int k = 0L; k < n; k++) {
    const int end = battery.length - n + k + 1L;

    int choosen_i;
    int cellvalue = 0L;

    for (int i = offset; i < end; i++) {
      const int x = battery.values[i] - '0';

      if (x <= cellvalue)
        continue;

      choosen_i = i;
      cellvalue = x;
    }

    offset = choosen_i + 1L;
    joltage = 10L * joltage + cellvalue;
  }

  return joltage;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector batteries = readfile(argv[1], '\n');

  printStringVector(batteries);

  int counter_t1 = 0L;
  unsigned long long counter_t2 = 0L;

  for (int i = 0L; i < batteries.length; i++) {
    String battery = batteries.values[i];

    counter_t1+=getJoltageT1(battery);
    counter_t2+=getJoltageT2(battery, 12);
    printf("Battery: ");
    printString(battery);
    printf(", joltage(1) = %2d", getJoltageT1(battery));
    printf(", joltage(2) = %12llu\n", getJoltageT2(battery, 12));
  }

  printf("Answer task 1: %d\n", counter_t1);
  printf("Answer task 2: %llu\n", counter_t2);
  freeStringVector(&batteries);
  
  return 0;
}
