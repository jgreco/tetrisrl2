#ifndef MONSTER_H_
#define MONSTER_H_

#include "tetromino.h"
#include <stdint.h>

enum DIRECTIONS { UP, LEFT, DOWN, RIGHT, UPLEFT, DOWNLEFT, DOWNRIGHT, UPRIGHT };
enum AI_TYPE { RANDWALK=1, RANGE=2, MELEE=3 };

typedef struct monster_struct {
	char *type;
	char *symbol;

	char *evade;
	char *strike;
	char *pickup;
	char *die;
	char *range_hit;
	char *range_miss;

	int hp, attack, defense;
	uint8_t dead;
	uint8_t ai;

	int cost;

	int x, y;
	uint8_t is_on_tetromino;
	uint8_t is_player;
	uint8_t is_object;
} monster;


void init_monsters();
monster *create_new_monster(char *type);
void move_monster(monster *mon, int x, int y);

int acceptable_move(monster *mon, int direction);
void transform_coord(int direction, int *x, int *y);

void ai(monster *mon);

void bump_attack(monster *attacker, monster *defender);

#endif
