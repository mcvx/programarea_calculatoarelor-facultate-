#pragma once

// Free a matrix with <linesCount> lines
void freeMatrix(void **m, int linesCount);

// Compare numbers function used in qsort
int compareNumbers(const void *a, const void *b);
