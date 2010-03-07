#include <stdio.h>

FILE *logging;

#define log_(...) fprintf(logging, __VA_ARGS__ ); fflush(logging)

#define DEBUGGING_AI

