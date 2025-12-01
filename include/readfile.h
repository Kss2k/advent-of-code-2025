#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include "vector.h"
#include <stdlib.h>


StringVector readfile(const char* path) {
  FILE *fptr = fopen(path, "r");

  if (fptr == NULL) {
    printf("Error opening file!\n");
    exit(-1);
  }

  char c;
  String line;
  StringVector lines = initStringVector();
  int newline = 1;

  while (1) {
    c = fgetc(fptr);
    if (newline) line = initString();

    if (c == '\n' || c == EOF) {
      if (line.length) appendStringVector(&lines, line);
      newline = 1;
    } else {
      newline = 0;
      appendString(&line, c);
    }

    if (c == EOF)
      break;
  }

  return lines;
}


#endif
