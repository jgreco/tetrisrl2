#ifndef LEVEL_H_
#define LEVEL_H_

#include "tetromino.h"

enum tile_types { VOID, VOID_W, RED, RED_W, GREEN, GREEN_W, YELLOW, YELLOW_W, BLUE, BLUE_W, MAGENTA, MAGENTA_W, CYAN, CYAN_W, WHITE, WHITE_W } ;

typedef struct level_struct {
	tetromino falling;

	int width, height;

	int data[82][42];

} level;

level *make_level();
void eliminate_rows(level *lev);

#endif
