#include "utils.h"

#include <stdlib.h>

void freeMatrix(void **m, int linesCount)
{
    int i;

    for (i = 0; i < linesCount; i++)
    {
        free(m[i]);
    }
    
    free(m);
}

int compareNumbers(const void *a, const void *b)
{
    return (*(int*)a - *(int*)b);
}
