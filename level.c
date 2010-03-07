#include "level.h"
#include <stdlib.h>

level *make_level()
{
	int i, k;

	level *new = (level *)malloc(sizeof(level));

	new->width = 42;
	new->height = 82;


	for(i=0; i<new->height; i++)
	for(k=0; k<new->width;  k++) {
		new->data[i][k] = VOID;
	}

	for(i=0; i<new->width; i++) {
		new->data[0][i] = VOID_W;
		new->data[new->height-1][i] = VOID_W;
	}
	for(i=0; i<new->height; i++) {
		new->data[i][0] = VOID_W;
		new->data[i][new->width-1] = VOID_W;
	}

	return new;
}
void eliminate_rows(level *lev)
{
	return;
}
