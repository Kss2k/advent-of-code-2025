#include "vector.h"
#include "readfile.h"
#include "math.h"


#define DEBUG 0

IntVector parseLine(String line) {
  IntVector numbers = initIntVector();

  int num         = 0;
  int initialized = 0;
  for (int i = 0; i < line.length; i++) {
    const char c = line.values[i];

    if (initialized && c == ' ') {
      appendIntVector(&numbers, num);
      num         = 0;
      initialized = 0;

    } else if (c != ' ') {
      num         = 10 * num + c - '0';
      initialized = 1;
    }
  }

  if (initialized) {
    appendIntVector(&numbers, num);
  }

  return numbers;
}


char asOperand(const int x) {
  return (char)(x + '0');
}


long long applyOperand(const long long x, const long long y, const int operand) {
  switch (asOperand(operand)) {
    case '*': {
      #if DEBUG 
        printf("%lld * %lld\n", x, y);
      #endif
      return x * y;
    }
    case '+': {
      #if DEBUG 
        printf("%lld + %lld\n", x, y);
      #endif
      return x + y;
    }
    default: {
      printf("Unexpected operand: %c!\n", asOperand(operand));
      exit(-1);
    }
  }
}


long long task2(StringVector lines) {
  String operandLine = lines.values[lines.length - 1];
  IntVector operands = initIntVector();
  int o = 0;
  for (int i = 0; i < operandLine.length; i++) {
    const char c = operandLine.values[i];
    if (c == '+' || c == '*') appendIntVector(&operands, (int)(c - '0'));
  }
  
  long long result = 0;
  long long cache  = 0;
  int initialized  = 0;

  for (int i = 0; i < lines.values[0].length; i++) {
    long long number = 0;

    for (int j = 0; j < lines.length - 1; j++) {
      const char c = lines.values[j].values[i];
      const int add = c != ' ';
      number = (!add) * number + add * (10 * number + c - '0');
    }

    if (!initialized && number > 0) {
      #if DEBUG
        printf("Number: %d\n", number);
      #endif
      initialized = 1;
      cache = number;

    } else if (number > 0) {
      #if DEBUG
        printf("Number: %d\n", number);
      #endif
      initialized = 1;
      cache = applyOperand(cache, number, operands.values[o]);

    } else if (number <= 0) {
      #if DEBUG
        printf("Cache: %d\n", cache);
      #endif
      result += cache;
      cache       = 0;
      initialized = 0;
      o++; // go to next operand
    }
  }

  if (initialized) result += cache;

  freeIntVector(&operands);
  return result;
}


long long columnCalculation(IntArray registry) {
  IntVector operands = registry.values[registry.length-1];

  long long result = 0;
  for (int j = 0; j < registry.values[0].length; j++) {
    long long counter_j = registry.values[0].values[j];
    int operand_j       = operands.values[j];

    for (int i = 1; i < registry.length - 1; i++) {
      #if DEBUG 
        printf("[%d, %d]\n", i, j);
      #endif
      counter_j = applyOperand(counter_j, registry.values[i].values[j], operand_j);  
    }

    result += counter_j;
  }

  return result;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector input = readfile(argv[1], '\n');
  IntArray registry = initIntArray();
  printStringVector(input);

  int counter_t1 = 0;

  for (int i = 0; i < input.length; i++) {
    String line = input.values[i];
    appendIntArray(&registry, parseLine(line));
  }
  printIntArray(registry);

  printf("Answer Task 1: %lld\n", columnCalculation(registry));
  printf("Answer Task 2: %lld\n", task2(input));
  freeStringVector(&input);
  freeIntArray(&registry);
  
  return 0;
}
