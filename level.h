#ifndef LEVEL_H_
#define LEVEL_H_

#include "tetromino.h"
#include "arraylist.h"


typedef struct level_struct {
	tetromino *falling;

	int width, height;
	arraylist monsters;

	int data[82][42];

} level;

level *make_level();
void eliminate_rows(level *lev);
void move_tetromino_down(level *lev);
void move_tetromino_right(level *lev);
void move_tetromino_left(level *lev);
void place_tetromino(level *lev);
void rotate_tetromino();

void gen_new_tetromino();

#endif
