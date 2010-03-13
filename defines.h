#ifndef DEFINES_H_
#define DEFINES_H_

#include "level.h"
#include "monster.h"
#include "arraylist.h"

//enum tile_types { VOID, VOID_W, RED, RED_W, GREEN, GREEN_W, YELLOW, YELLOW_W, BLUE, BLUE_W, MAGENTA, MAGENTA_W, CYAN, CYAN_W, WHITE, WHITE_W } ;
enum tile_types { VOID, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, VOID_W, RED_W, GREEN_W, YELLOW_W, BLUE_W, MAGENTA_W, CYAN_W, WHITE_W } ;

level *board;
monster *player;
arraylist messages;

int tetromino_allowance;
int lines_cleared;
int stop_tetrominos;
int score;

#endif
