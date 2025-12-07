#include "vector.h"
#include "readfile.h"
#include "math.h"


#define DEBUG 0


typedef struct {
  StringVector grid;
  LongLongIntArray timeLines;
  int iter;
  long long splits;
  long long numTimeLines;
} Grid;


Grid initGrid(StringVector lines) {
  Grid grid;
  grid.grid = lines;
  grid.timeLines = initLongLongIntArray();

  for (int i = 0; i < lines.length; i++) {
    LongLongIntVector intLine = initLongLongIntVector();
    String strLine = lines.values[i];

    for (int j = 0; j < strLine.length; j++) {
      const char c = strLine.values[j];
      const long long val = 1 * (c == 'S');
      appendLongLongIntVector(&intLine, val);
    }

    appendLongLongIntArray(&grid.timeLines, intLine);
  }

  grid.iter = 1; // 0th iteration has already happened
  grid.splits = 0;
  grid.numTimeLines = 0;

  return grid;
}


void printGrid(const Grid grid) {
  printf("Grid [iter = %d]:\n", grid.iter);
  printStringVector(grid.grid);

  printf("Timelines [iter = %d]:\n", grid.iter);
  for (int i = 0; i < grid.timeLines.length; i++) {
    long long n = 0;

    for (int j = 0; j < grid.timeLines.values[i].length; j++) {
      const long long v = grid.timeLines.values[i].values[j];
      n+=v;

      if (!v) printf(" .");
      else    printf("%2lld", v);
    }

    printf(" [%3d]\n", n);
  }
}


char getGridField(const Grid *grid, const int i, const int j) {
  if (i < 0 || i >= grid->grid.length ||
      j < 0 || j >= grid->grid.values[i].length)
    return '.';

  return grid->grid.values[i].values[j];
}


void setGridField(Grid *grid, const int i, const int j, const char c, const long long n) {
  if (i < 0 || i >= grid->grid.length ||
      j < 0 || j >= grid->grid.values[i].length)
    return;
 
  grid->numTimeLines += n;
  grid->timeLines.values[i].values[j] += n;
  grid->grid.values[i].values[j] = c;
}


int iterGrid(Grid *grid) {
  if (grid->iter >= grid->grid.length)
    return 0;

  const int i0 = grid->iter - 1, i = grid->iter;
  StringVector G = grid->grid;
  grid->numTimeLines = 0;

  for (int j = 0; j < G.values[i].length; j++) {
    const char above   = getGridField(grid, i0, j);
    const char current = getGridField(grid, i, j);
    const int abovestream = above == '|' || above == 'S';
    const long long aboveTimeLines = grid->timeLines.values[i0].values[j];

    if (abovestream && current == '^') {
      grid->splits++;
      setGridField(grid, i, j - 1, '|', aboveTimeLines);
      setGridField(grid, i, j + 1, '|', aboveTimeLines);
    } else if (abovestream) {
      setGridField(grid, i, j, '|', aboveTimeLines);
    }
  }

  grid->iter++;
  return 1;
}


void freeGrid(Grid *grid) {
  grid->iter = 0;
  freeStringVector(&grid->grid);
  freeLongLongIntArray(&grid->timeLines);
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  Grid grid = initGrid(readfile(argv[1], '\n'));

  while (iterGrid(&grid))
    printGrid(grid);

  printf("Answer Task 1: %lld\n", grid.splits);
  printf("Answer Task 2: %lld\n", grid.numTimeLines);
  freeGrid(&grid);
  
  return 0;
}
