#ifndef MONSTER_H_
#define MONSTER_H_

#include "tetromino.h"
#include <stdint.h>

typedef struct monster_struct {
	char *type;
	char *symbol;

	int hp, attack, defense;
	int dead;

	int cost;

	int x, y;
	uint8_t is_on_tetromino;
	uint8_t is_player;
} monster;

void init_monsters();
monster *create_new_monster(char *type);
void move_monster(monster *mon, int x, int y);

#endif
