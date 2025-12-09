#include "vector.h"
#include "readfile.h"
#include "math.h"


#define DEBUG 0


typedef struct {
  long x;
  long y;
} Point;


typedef struct {
  Point p0;
  Point p1;
  int vertical;
  int horizontal;
} Line;


typedef struct {
  Point A;
  Point C;
  Point B;
  Point D;
  Line ab;
  Line bc;
  Line cd;
  Line da;
} Rectangle;


void printLine(const Line line);


static inline long minl(const long a, const long b) { return a < b ? a : b; }
static inline long maxl(const long a, const long b) { return a > b ? a : b; }


static inline int pointsEqual(const Point a, const Point b) {
  return a.x == b.x && a.y == b.y;
}


Line createLine(const Point p0, const Point p1) {
  Line line;
  line.p0 = p0;
  line.p1 = p1;
  line.vertical = p0.x == p1.x && p0.y != p1.y;
  line.horizontal = p0.y == p1.y && p0.x != p1.x;
  return line;
}


Rectangle createRectangle(Point A, Point C) {
  // A and C are opposite corners
  Rectangle box;

  // Shrnk box to make boundary checking easier
  if (A.x > C.x) {
    A.x--;
    C.x++;
  } else {
    A.x++;
    C.x--;
  }
  
  if (A.y > C.y) {
    A.y--;
    C.y++;
  } else {
    A.y++;
    C.y--;
  }

  Point B;
  B.x = A.x;
  B.y = C.y;

  Point D;
  D.x = C.x;
  D.y = A.y;

  box.A = A;
  box.B = B;
  box.C = C;
  box.D = D;

  box.ab = createLine(A, B);
  box.bc = createLine(B, C);
  box.cd = createLine(C, D);
  box.da = createLine(D, A);

  return box;
}


void printLine(const Line line) { 
  const char type = line.vertical ? 'v' : line.horizontal ? 'h' : '?';
  printf("(%ld,%ld)->(%ld,%ld) [%c]\n", line.p0.x, line.p0.y, line.p1.x, line.p1.y, type);
}
void freeLine(const Line* line) { return; }
CREATE_VECTOR_ASSETS(Line, LineVector, printLine, 0, freeLine);

static inline int signl(const long value) {
  return (value > 0) - (value < 0);
}



int lineCrossesLines(const Line line, const LineVector lines) {
  if (!(line.vertical || line.horizontal)) {
    #if DEBUG
      printf("line is not horizontal or vertical! ");
      printLine(line);
    #endif
    return 0; 
  }

  for (int i = 0; i < lines.length; i++) {
    const Line opposing = lines.values[i];

    if (opposing.vertical == line.vertical &&
        opposing.horizontal == line.horizontal) {
      continue;
    }

    if (line.vertical && opposing.horizontal) {
      const long lineX = line.p0.x;
      const long minOppX = minl(opposing.p0.x, opposing.p1.x);
      const long maxOppX = maxl(opposing.p0.x, opposing.p1.x);
      const long oppY = opposing.p0.y;
      const long minLineY = minl(line.p0.y, line.p1.y);
      const long maxLineY = maxl(line.p0.y, line.p1.y);

      if (lineX >= minOppX && lineX <= maxOppX &&
          oppY >= minLineY && oppY <= maxLineY)
        return 1;

    } else if (line.horizontal && opposing.vertical) {
      const long lineY = line.p0.y;
      const long minOppY = minl(opposing.p0.y, opposing.p1.y);
      const long maxOppY = maxl(opposing.p0.y, opposing.p1.y);
      const long oppX = opposing.p0.x;
      const long minLineX = minl(line.p0.x, line.p1.x);
      const long maxLineX = maxl(line.p0.x, line.p1.x);

      if (lineY >= minOppY && lineY <= maxOppY &&
          oppX >= minLineX && oppX <= maxLineX)
        return 1;
    }

  }

  return 0;
}


int rectangleCrossesLines(Rectangle box, LineVector lines) {
  return (
    lineCrossesLines(box.ab, lines) ||
    lineCrossesLines(box.bc, lines) ||
    lineCrossesLines(box.cd, lines) ||
    lineCrossesLines(box.da, lines)
  );
}


void printPoint(const Point point) { printf("(%ld,%ld)\n", point.x, point.y); }
void freePoint(const Point* point) { return; }
CREATE_VECTOR_ASSETS(Point, PointVector, printPoint, 0, freePoint);

void parseInputLine(PointVector* points, const String inputLine) {
  Point point;

  long num = 0;
  for (int i = 0; i < inputLine.length; i++) {
    const char c = inputLine.values[i];

    if (c == ',') {
      point.x = num;
      num = 0;
      continue;
    }

    num = 10 * num + c - '0';
  }

  point.y = num;

  appendPointVector(points, point);
}


LineVector createLines(PointVector points) {
  LineVector lines = initLineVector();

  for (int i = 0; i < points.length; i++) {
    const int j = i == 0 ? points.length - 1 : i - 1;
    const Point p0 = points.values[j];
    const Point p1 = points.values[i];
    Line line = createLine(p0, p1);
    appendLineVector(&lines, line);
  }

  return lines;
}


long long absll(const long long x) {
  return x < 0 ? -1 * x : x;
}


long long calcArea(const Point p0, const Point p1) {
  return (absll(p1.x - p0.x) + 1) * (absll(p1.y - p0.y) + 1);
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need file argument!\n");
    return -1;
  }

  StringVector input = readfile(argv[1], '\n');
  PointVector points = initPointVector();

  for (int i = 0; i < input.length; i++)
    parseInputLine(&points, input.values[i]);

  LineVector lines = createLines(points);

  printStringVector(input);
  printPointVector(points);
  printLineVector(lines);
 
  // Task 1
  long long maxArea1 = 0;
  for (int i = 0; i < points.length; i++) {
    const Point p0 = points.values[i];

    for (int j = 0; j < i; j++) {
      const Point p1 = points.values[j];
      const long long area = calcArea(p0, p1); 
      #if DEBUG
        printf("IDX:%d,%d, P0:(%d,%d), P1:(%d,%d) Area: %lld\n", i, j, p0.x, p0.y, p1.x, p1.y, area);
      #endif
      if (area > maxArea1) maxArea1 = area;
    }
  }

  printf("Answer Task 1: %lld\n", maxArea1);
  
  // Task 2
  long long maxArea2 = 0;
  for (int i = 0; i < points.length; i++) {
    const Point p0 = points.values[i];

    for (int j = 0; j < i; j++) {
      const Point p1 = points.values[j];
      const long long area = calcArea(p0, p1); 
      const Rectangle box = createRectangle(p0, p1);
      const int crosses = rectangleCrossesLines(box, lines);

      #if DEBUG
        printf("IDX:%d,%d, P0:(%d,%d), P1:(%d,%d) Crosses: %d, Area: %lld\n", i, j, p0.x, p0.y, p1.x, p1.y, crosses, area);
      #endif
      if (!crosses && area > maxArea2) maxArea2 = area;
    }
  }
  printf("Answer Task 2: %lld\n", maxArea2);

  freePointVector(&points);
  freeLineVector(&lines);
  freeStringVector(&input);
  
  return 0;
}
