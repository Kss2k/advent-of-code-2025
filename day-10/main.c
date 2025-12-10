#include "vector.h"
#include "readfile.h"
#include "math.h"
#include <stdint.h>
#include <limits.h>

#define DEBUG 0
#define ITER_MAX 500
#define CACHE_SIZE 500000

#define UINT uint64_t
#define MAX_DIGITS 16
#define MAX_BUTTONS 16
#define MAX_FREE_VARS 4

typedef struct {
  long long num;
  long long den;
} Fraction;


static long long llabsll(long long x) {
  return x < 0 ? -x : x;
}


static __int128 i128_abs(__int128 x) {
  return x < 0 ? -x : x;
}


static __int128 gcd128(__int128 a, __int128 b) {
  a = i128_abs(a);
  b = i128_abs(b);

  while (b != 0) {
    __int128 t = a % b;
    a = b;
    b = t;
  }

  return a;
}


static Fraction fraction_from_i128(__int128 num, __int128 den) {
  if (den == 0) {
    Fraction out = {0, 1};
    return out;
  }

  if (den < 0) {
    num = -num;
    den = -den;
  }

  __int128 g = gcd128(num, den);
  num /= g;
  den /= g;

  Fraction out = {(long long)num, (long long)den};
  return out;
}


static Fraction fraction_from_int(long long value) {
  Fraction out = {value, 1};
  return out;
}


static Fraction fraction_add(Fraction a, Fraction b) {
  __int128 num = (__int128)a.num * b.den + (__int128)b.num * a.den;
  __int128 den = (__int128)a.den * b.den;
  return fraction_from_i128(num, den);
}


static Fraction fraction_sub(Fraction a, Fraction b) {
  __int128 num = (__int128)a.num * b.den - (__int128)b.num * a.den;
  __int128 den = (__int128)a.den * b.den;
  return fraction_from_i128(num, den);
}


static Fraction fraction_mul(Fraction a, Fraction b) {
  __int128 num = (__int128)a.num * b.num;
  __int128 den = (__int128)a.den * b.den;
  return fraction_from_i128(num, den);
}


static Fraction fraction_div(Fraction a, Fraction b) {
  __int128 num = (__int128)a.num * b.den;
  __int128 den = (__int128)a.den * b.num;
  return fraction_from_i128(num, den);
}


static int fraction_is_zero(Fraction a) {
  return a.num == 0;
}


static int fraction_less(Fraction a, Fraction b) {
  __int128 left = (__int128)a.num * b.den;
  __int128 right = (__int128)b.num * a.den;
  return left < right;
}


static int fraction_greater(Fraction a, Fraction b) {
  __int128 left = (__int128)a.num * b.den;
  __int128 right = (__int128)b.num * a.den;
  return left > right;
}


void freeUINT(const UINT* x) { return; } // just stops compile errors, its never called
void printUINT(const UINT x) { printf("%u\n", x); }
CREATE_VECTOR_ASSETS(UINT, UINT_Vector, printUINT, 0, freeUINT);

void printButton(const UINT_Vector button) {
  printf("(");
  for (int i = 0; i < button.length; i++)
    printf("%lld,", button.values[i]);
  printf(")\n");
}
CREATE_VECTOR_ASSETS(UINT_Vector, UINT_Array, printButton, 1, freeUINT_Vector);


UINT toggleIthBit(UINT indicatorLight, const unsigned i) {
  return indicatorLight ^ (1 << i); 
}


typedef struct {
  UINT targetIndicatorLight;
  UINT currentIndicatorLight;
  UINT_Vector buttons;
  UINT_Array Buttons;
  IntVector joltages;
  UINT digits;
  int npresses;
} Machine;


Machine createMachine(const String line) {
  Machine machine;

  int i = 0;
  int k = 0;
  char c = '0';
  UINT targetIndicatorLight = 0;
  UINT currentIndicatorLight = 0;

  // Read [.##.]
  for (; i < line.length && c != '('; i++) {
    c = line.values[i];

    if (c == '#')
      targetIndicatorLight = toggleIthBit(targetIndicatorLight, k);

    k += c=='#'||c=='.';
  }

  // Read (3), (1,3) (2) (0,2) ...
  UINT_Vector buttons = initUINT_Vector();
  UINT_Vector Button = initUINT_Vector();
  UINT_Array Buttons = initUINT_Array();
  UINT button = 0;
  UINT digit  = 0;
  for (; i < line.length && c != '{'; i++) {
    c = line.values[i];

    if (c == ',') {
      button = toggleIthBit(button, digit); 
      appendUINT_Vector(&Button, digit);

      digit  = 0;

    } else if (c == ')') {
      button = toggleIthBit(button, digit); 

      appendUINT_Vector(&Button, digit);
      appendUINT_Array(&Buttons, Button);
      appendUINT_Vector(&buttons, button);
      
      Button = initUINT_Vector();
      button = 0;
      digit  = 0;

    } else if (c != '(' & c != ' ') {
      digit = 10 * digit + c - '0';
    }
  }

  if (Button.length == 0)
    freeUINT_Vector(&Button);

  // Read (3), (1,3) (2) (0,2) ...
  IntVector joltages = initIntVector();
  int joltage = 0;
  for (; i < line.length; i++) {
    c = line.values[i];

    if (c == ',' || c == '}') {
      appendIntVector(&joltages, joltage);
      joltage = 0;

    } else if (c != ' ') {
      joltage = 10 * joltage + c - '0';
    }
  }

  // We can reverse this, which makes caching possible across
  // machines, as they all have the same target, but different current values
  // machine.targetIndicatorLight  = targetIndicatorLight;
  // machine.currentIndicatorLight = currentIndicatorLight;
  machine.targetIndicatorLight  = targetIndicatorLight;
  machine.currentIndicatorLight = currentIndicatorLight;
  machine.buttons               = buttons;
  machine.joltages              = joltages;
  machine.digits                = k;
  machine.npresses              = 0;
  machine.Buttons               = Buttons;

  return machine;
}


void freeMachine(Machine *machine) {
  freeUINT_Vector(&machine->buttons);
  freeIntVector(&machine->joltages);
  freeUINT_Array(&machine->Buttons);
}


void printMachine(Machine machine) {
  // Target
  printf("T:[");
  for (int i = 0; i < machine.digits; i++) {
    const char c = (machine.targetIndicatorLight >> i) & 1 ? '#' : '.';
    printf("%c", c);
  }
  printf("] ");

  // Current
  printf("C:[");
  for (int i = 0; i < machine.digits; i++) {
    const char c = (machine.currentIndicatorLight >> i) & 1 ? '#' : '.';
    printf("%c", c);
  }

  printf("] ");
  // Buttons
  for (int b = 0; b < machine.buttons.length; b++) {
    UINT button = machine.buttons.values[b];

    printf("(");
    for (int i = 0; i < machine.digits; i++) {
      if ((button >> i) & 1) {
        printf("%d,", i);
      }
    }
    printf(") ");
  }
  
  printf("{");
  // Buttons
  for (int j = 0; j < machine.joltages.length; j++) {
    int joltage = machine.joltages.values[j];
    printf("%d,", joltage);
  }
  printf("}\n");
}


CREATE_VECTOR_ASSETS(Machine, MachineVector, printMachine, 1, freeMachine);


UINT IS_IN_CACHE[CACHE_SIZE];
UINT PRESS_CACHE[CACHE_SIZE];

void resetCache() {
  for (int i = 0; i < CACHE_SIZE; i++) {
    IS_IN_CACHE[i] = 0;
    PRESS_CACHE[i] = 0;
  }
}


long long findFewestPressesT1(const Machine machine, const long long max) {
  if (machine.currentIndicatorLight >= CACHE_SIZE)
    printf("Out of bounds currentIndicatorLight: %lld\n", machine.currentIndicatorLight);

  if (IS_IN_CACHE[machine.currentIndicatorLight]) {
    const long long val = PRESS_CACHE[machine.currentIndicatorLight];
    #if DEBUG
      printf("EXTRACTING | id: %lld, val: %lld\n", machine.currentIndicatorLight, val);
    #endif
    return machine.npresses + val;
  }

  if (machine.targetIndicatorLight == machine.currentIndicatorLight) {
    return machine.npresses;

  } else if (machine.npresses >= machine.buttons.length || machine.npresses >= max) {
    #if DEBUG
      printf("Hit iteration maximum!\n");
    #endif

    return machine.npresses + LLONG_MAX / 2;
  }

  long long minPresses = LLONG_MAX / 2;
  // 1. I don't think the ordering of the buttons matter
  // 2. I don't think we should ever press a button twice
  //    as it just undoes the inital button press...
  //    thus we can just iterate recursively in order?
  for (int b = 0; b < machine.buttons.length; b++) {
    const UINT button = machine.buttons.values[b];

    Machine nextMachine = machine;
    nextMachine.npresses++;
    nextMachine.currentIndicatorLight = nextMachine.currentIndicatorLight ^ button;

    #if DEBUG
      printf("npresses: %d, button: %d\n", nextMachine.npresses, b + 1);
      printf("Machine:\n");
      printMachine(nextMachine);
    #endif

    const long long presses = findFewestPressesT1(nextMachine, minPresses);
    const long long diffPresses = presses - nextMachine.npresses;

    if (diffPresses < LLONG_MAX / 4) {
      #if DEBUG
        printf("CACHING    | id: %lld, val: %lld\n", nextMachine.currentIndicatorLight, diffPresses);
      #endif
      IS_IN_CACHE[nextMachine.currentIndicatorLight] = 1;
      PRESS_CACHE[nextMachine.currentIndicatorLight] = diffPresses;
    }

    if (presses < minPresses)
      minPresses = presses;
  }

  return minPresses;
}


typedef struct {
  const Machine *machine;
  int rank;
  int free_count;
  int free_indices[MAX_FREE_VARS];
  long long free_limits[MAX_FREE_VARS];
  long long free_values[MAX_FREE_VARS];
  int pivot_cols[MAX_DIGITS];
  long long pivot_limits[MAX_DIGITS];
  Fraction coeff[MAX_DIGITS][MAX_FREE_VARS];
  Fraction row_value[MAX_DIGITS];
  long long best;
} Task2Context;


static int validateSolution(const Machine *machine, const long long *counts) {
  for (int digit = 0; digit < machine->joltages.length; digit++) {
    long long total = 0;

    for (int b = 0; b < machine->Buttons.length; b++) {
      const long long presses = counts[b];
      if (presses == 0)
        continue;

      const UINT_Vector button = machine->Buttons.values[b];
      for (int k = 0; k < button.length; k++) {
        if (button.values[k] == (UINT)digit)
          total += presses;
      }
    }

    if (total != machine->joltages.values[digit])
      return 0;
  }

  return 1;
}


static void dfsTask2(Task2Context *ctx, long long partial_sum, int idx, long long *counts) {
  if (partial_sum >= ctx->best)
    return;

  if (idx == ctx->free_count) {
    long long total = partial_sum;

    for (int r = 0; r < ctx->rank; r++) {
      const Fraction value = ctx->row_value[r];
      if (value.den != 1)
        return;

      const long long val = value.num;
      if (val < 0 || val > ctx->pivot_limits[r])
        return;

      counts[ctx->pivot_cols[r]] = val;
      total += val;
    }

    if (!validateSolution(ctx->machine, counts))
      return;

    if (total < ctx->best)
      ctx->best = total;

    return;
  }

  const int column = ctx->free_indices[idx];
  const long long limit = ctx->free_limits[idx];

  for (long long presses = 0; presses <= limit; presses++) {
    ctx->free_values[idx] = presses;
    counts[column] = presses;

    Fraction deltas[MAX_DIGITS];
    const Fraction pressFrac = fraction_from_int(presses);
    for (int r = 0; r < ctx->rank; r++) {
      deltas[r] = fraction_mul(ctx->coeff[r][idx], pressFrac);
      ctx->row_value[r] = fraction_sub(ctx->row_value[r], deltas[r]);
    }

    int feasible = 1;
    for (int r = 0; r < ctx->rank && feasible; r++) {
      Fraction min_possible = ctx->row_value[r];
      Fraction max_possible = ctx->row_value[r];

      for (int next = idx + 1; next < ctx->free_count; next++) {
        const Fraction coeff = ctx->coeff[r][next];
        if (fraction_is_zero(coeff))
          continue;

        const Fraction limitFrac = fraction_from_int(ctx->free_limits[next]);
        const Fraction impact = fraction_mul(coeff, limitFrac);

        if (coeff.num > 0)
          min_possible = fraction_sub(min_possible, impact);
        else if (coeff.num < 0)
          max_possible = fraction_sub(max_possible, impact);
      }

      const Fraction zeroFrac = fraction_from_int(0);
      if (fraction_less(max_possible, zeroFrac))
        feasible = 0;

      const Fraction limitFrac = fraction_from_int(ctx->pivot_limits[r]);
      if (fraction_greater(min_possible, limitFrac))
        feasible = 0;
    }

    if (feasible)
      dfsTask2(ctx, partial_sum + presses, idx + 1, counts);

    for (int r = 0; r < ctx->rank; r++)
      ctx->row_value[r] = fraction_add(ctx->row_value[r], deltas[r]);
  }

  counts[column] = 0;
  ctx->free_values[idx] = 0;
}


long long solveFewestPressesT2(const Machine *machine) {
  if (machine->joltages.length > MAX_DIGITS || machine->Buttons.length > MAX_BUTTONS)
    return LLONG_MAX / 2;

  Fraction matrix[MAX_DIGITS][MAX_BUTTONS + 1];
  const Fraction zero = fraction_from_int(0);
  for (int i = 0; i < MAX_DIGITS; i++) {
    for (int j = 0; j < MAX_BUTTONS + 1; j++)
      matrix[i][j] = zero;
  }

  const int digits = machine->joltages.length;
  const int buttons = machine->Buttons.length;

  for (int row = 0; row < digits; row++)
    matrix[row][buttons] = fraction_from_int(machine->joltages.values[row]);

  for (int col = 0; col < buttons; col++) {
    const UINT_Vector Button = machine->Buttons.values[col];
    for (int k = 0; k < Button.length; k++)
      matrix[Button.values[k]][col] = fraction_from_int(1);
  }

  int pivot_cols[MAX_DIGITS] = {0};
  int rank = 0;
  for (int col = 0; col < buttons && rank < digits; col++) {
    int pivot = -1;
    for (int row = rank; row < digits; row++) {
      if (!fraction_is_zero(matrix[row][col])) {
        pivot = row;
        break;
      }
    }

    if (pivot < 0)
      continue;

    if (pivot != rank) {
      for (int c = 0; c < buttons + 1; c++) {
        Fraction tmp = matrix[pivot][c];
        matrix[pivot][c] = matrix[rank][c];
        matrix[rank][c] = tmp;
      }
    }

    const Fraction pivotVal = matrix[rank][col];
    for (int c = col; c < buttons + 1; c++)
      matrix[rank][c] = fraction_div(matrix[rank][c], pivotVal);

    for (int row = 0; row < digits; row++) {
      if (row == rank)
        continue;

      const Fraction factor = matrix[row][col];
      if (fraction_is_zero(factor))
        continue;

      for (int c = col; c < buttons + 1; c++) {
        const Fraction product = fraction_mul(factor, matrix[rank][c]);
        matrix[row][c] = fraction_sub(matrix[row][c], product);
      }
    }

    pivot_cols[rank] = col;
    rank++;
  }

  for (int row = rank; row < digits; row++) {
    int zeroRow = 1;
    for (int col = 0; col < buttons; col++) {
      if (!fraction_is_zero(matrix[row][col])) {
        zeroRow = 0;
        break;
      }
    }

    if (zeroRow && !fraction_is_zero(matrix[row][buttons]))
      return LLONG_MAX / 2;
  }

  int isPivot[MAX_BUTTONS] = {0};
  for (int r = 0; r < rank; r++)
    isPivot[pivot_cols[r]] = 1;

  int free_indices[MAX_FREE_VARS] = {0};
  int free_count = 0;
  for (int col = 0; col < buttons; col++) {
    if (!isPivot[col]) {
      if (free_count >= MAX_FREE_VARS)
        return LLONG_MAX / 2;
      free_indices[free_count++] = col;
    }
  }

  long long button_limits[MAX_BUTTONS] = {0};
  for (int col = 0; col < buttons; col++) {
    int limit = INT_MAX;
    const UINT_Vector Button = machine->Buttons.values[col];

    if (Button.length == 0) {
      limit = 0;
    } else {
      for (int k = 0; k < Button.length; k++) {
        const int digit = Button.values[k];
        const int need = machine->joltages.values[digit];
        if (need < limit)
          limit = need;
      }
    }

    if (limit == INT_MAX)
      limit = 0;

    button_limits[col] = limit;
  }

  Task2Context ctx;
  ctx.machine = machine;
  ctx.rank = rank;
  ctx.free_count = free_count;
  ctx.best = LLONG_MAX / 2;

  for (int i = 0; i < free_count; i++) {
    ctx.free_indices[i] = free_indices[i];
    ctx.free_limits[i] = button_limits[free_indices[i]];
    ctx.free_values[i] = 0;
  }

  for (int r = 0; r < rank; r++) {
    ctx.pivot_cols[r] = pivot_cols[r];
    ctx.pivot_limits[r] = button_limits[pivot_cols[r]];
    ctx.row_value[r] = matrix[r][buttons];

    for (int j = 0; j < free_count; j++)
      ctx.coeff[r][j] = matrix[r][free_indices[j]];
  }

  long long counts[MAX_BUTTONS] = {0};
  dfsTask2(&ctx, 0, 0, counts);

  return ctx.best;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector input = readfile(argv[1], '\n');
  MachineVector machines = initMachineVector();

  for (int i = 0; i < input.length; i++) {
    const String line = input.values[i];
    const Machine machine = createMachine(line);
    appendMachineVector(&machines, machine);
  }
 
  #if DEBUG
    printf("\nInput:\n");
    printStringVector(input);
    printf("\nMachines:\n");
    printMachineVector(machines);
  #endif

  long long counter_t1 = 0, counter_t2 = 0;
  for (int i = 0; i < machines.length; i++) {
    resetCache();
    const long long presses_t1 = findFewestPressesT1(machines.values[i], ITER_MAX);

    const long long presses_t2 = solveFewestPressesT2(&machines.values[i]);
    
    counter_t1 += presses_t1;
    counter_t2 += presses_t2;
    printf("npresses %dth machine: %lld\n", i+1, presses_t1);
  }

  printf("Answer Task 1: %lld\n", counter_t1);
  printf("Answer Task 2: %lld\n", counter_t2);

  freeStringVector(&input);
  freeMachineVector(&machines);
  
  return 0;
}
