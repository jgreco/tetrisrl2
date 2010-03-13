#include "util.h"


void rotate_coords(int *x, int *y, int n)
{
	int old_x = *x;
	int old_y = *y;

	*x = (n-1) - old_y;
	*y = old_x;

	return;
}

