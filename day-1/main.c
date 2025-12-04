#include "vector.h"
#include "readfile.h"
#include "math.h"


int parseLine(const String line) {
  if (line.length <= 0) return 0;

  const int sign = line.values[0] == 'L' ? -1 : 1;

  int val = 0;
  for (int i = 1; i < line.length; i++)
    val = 10 * val + (int)(line.values[i] - '0');

  return sign * val;
}


int updateDial(const int dial, const int offset) {
  const int result = (dial + offset) % 100;
  return result < 0 ? result + 100 : result;
}


int countZeroPasses(int dial, const int offset) {
  const int sign = offset < 0 ? -1 : 1;
  const int magnitude = sign * offset;

  int counter = 0;
  for (int i = 0; i < magnitude; i++) {
    dial = dial + sign;
    if (dial < 0) dial  = 100 + dial;
    else if (dial > 99) dial = dial - 100;
    counter+=(dial==0);
  }

  return counter;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector lines = readfile(argv[1], '\n');

  int dial = 50;
  int counter_p1 = 0;
  int counter_p2 = 0;

  int j = -1;
  for (int i = 0; i < lines.length; i++) {
    String line = lines.values[i];
 
    const int offset = parseLine(line);
    const int numerator = dial + offset - (offset < 0);
    
    int passes0 = floor((double)(numerator) / 100.0);
    passes0 = passes0 < 0 ? -1 * passes0 : passes0;
    passes0 = passes0 && dial == 0 && offset < 0 ? passes0 - 1 : passes0;

    counter_p1 += (dial = updateDial(dial, offset))==0;
    counter_p2 += passes0;
  }

  printf("Answer Task 1 = %d\n", counter_p1);
  printf("Answer Task 2 = %d\n", counter_p2);
  freeStringVector(&lines);
  
  return 0;
}
