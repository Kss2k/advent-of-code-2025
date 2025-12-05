#include "vector.h"
#include "readfile.h"
#include "math.h"


int isPaperRoll(const StringVector *grid, const int i, const int j) {
  const int nrows = grid->length;
  const int ncols = grid->values[i].length;

  return
    i >= 0 && i < nrows &&
    j >= 0 && j < ncols &&
    grid->values[i].values[j] == '@';
}


int countAdjacentPaperRolls(const StringVector *grid, const int i0, const int j0) {
  #define N_STEPS 3
  const int steps[N_STEPS] = {-1, 0, 1};

  int paperRolls = 0;

  int visited = 0;
  for (int i = 0; i < N_STEPS; i++) {
    const int i1 = i0 + steps[i];

    for (int j = 0; j < N_STEPS; j++) {
      if (steps[i] == 0 && steps[j] == 0) continue;
      visited++;

      const int j1 = j0 + steps[j];
      paperRolls += isPaperRoll(grid, i1, j1);    
    }
  }

  return paperRolls;
}

int isMovablePaperRoll(const StringVector *grid, const int i, const int j) {
  #define ROLL_THRESHOLD 4

  return
    grid->values[i].values[j] == '@' &&
    countAdjacentPaperRolls(grid, i, j) < ROLL_THRESHOLD;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector grid = readfile(argv[1], '\n');
  printStringVector(grid);

  long long counter_t1 = 0;

  const int nrows = grid.length;
  const int ncols = grid.values[0].length;

  // Task 1
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < ncols; j++) {
       const int movable = isMovablePaperRoll(&grid, i, j);
       counter_t1+=movable;
    }
  }
  

  // Task 2
  long long counter_t2 = 0;
  int diff = 999;
  while (diff) {

    diff = 0;
    for (int i = 0; i < nrows; i++) {
      for (int j = 0; j < ncols; j++) {
        const int movable = isMovablePaperRoll(&grid, i, j);

        if (movable)
          grid.values[i].values[j] = 'x';

        counter_t2+=movable;
        diff      +=movable;
      }
    }
  }

  printf("Xgrid:\n");
  printStringVector(grid);

  printf("Answer task 1: %lld\n", counter_t1);
  printf("Answer task 2: %lld\n", counter_t2);
  freeStringVector(&grid);
  
  return 0;
}
