#include "vector.h"
#include "readfile.h"
#include "math.h"


typedef struct {
  long long lower;
  long long upper;
  String    slower;
  String    supper;
} Interval;


Interval parseInterval(String str) {
  Interval interval;
  interval.lower  = 0;
  interval.upper  = 0;
  interval.slower = initString();
  interval.supper = initString();

  long long* ptr = &interval.lower;
  String*   sptr = &interval.slower;

  char c;
  for (int i = 0; i < str.length; i++) {
    if ((c = str.values[i]) == '-') {
      ptr++; // go to upper
      sptr++;
      continue;
    }

    *ptr = 10 * (*ptr) + c - '0';
    appendString(sptr, c);
  }

  return interval;
}


int hasRepeatingPattern(String s) {
  const int K = s.length / 2;

  for (int k = 1; k <= K; k++) {
    if (s.length % k != 0) continue; // cannot be a repeating pattern with lenght k

    int match = s.length / k > 1; // should always be 1, but just in case
    int i = 0;

    // 123123123
    // k = 3
    // i = 0:
    //   0, 3, 6
    // i = 0
    for (int j = 1; j < s.length / k; j++) // iterate through the ith element of the k s.length/k patterns
      match &= s.values[(j - 1) * k + i] == s.values[j * k + i];

    if (match)
      return 1;
  }

  return 0;
}


int lessEqualStrings(String x, String y) {
  for (int i = 0; i < x.length; i++) {
    if      (x.values[i] < y.values[i]) return 1;
    else if (x.values[i] <= y.values[i]) continue;
    else return 0;
  }

  return 1;
}


String incrementString(String x) {
  for (int i = x.length - 1; i > 0; i--) {
    char c = x.values[i];

    if (c >= '9') {
      x.values[i] = '0';
      continue;
    } else x.values[i]++;
  }

  return x;
}


StringVector getIntervalElements(Interval interval) {
  StringVector elems = initStringVector();
  appendStringVector(&elems, interval.slower);

  String s;
  while (lessEqualStrings((s = incrementString(copyString(interval.slower))), interval.supper))
    appendStringVector(&elems,  s);

  freeString(&s);

  return elems;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector lines = readfile(argv[1], ',');

  printStringVector(lines);

  long counter_t1 = 0;

  for (int i = 0; i < lines.length; i++) {
    Interval interval = parseInterval(lines.values[i]);
    printf("line: %3d, lower: %12d, upper: %12d\n", i, interval.lower, interval.upper);
    // printString(interval.slower);
    // printf(", supper: ");
    // printString(interval.supper);
    // printf("\n");
    printStringVector(getIntervalElements(interval));

    counter_t1 += (
        hasRepeatingPattern(interval.slower) * interval.lower +
        hasRepeatingPattern(interval.supper) * interval.upper
    );
  }

  printf("Answer task 1: %ld\n", counter_t1);
  freeStringVector(&lines);
  
  return 0;
}
