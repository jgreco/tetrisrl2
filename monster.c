#include "monster.h"
#include <iniparser.h>
#include "hashtable.h"
#include <string.h>
#include "defines.h"

static struct llist_str *monsters_dict[HASHSIZE];

static char *construct_key(char *section, char *keyword);
char buf[STRN_SIZE];


void init_monsters()
{
	dictionary *monst_file;
	int i, num_entries;
	monster *new_monst;

	monst_file = iniparser_load("monsters.ini");
	num_entries = iniparser_getnsec(monst_file);

	for(i=0; i<num_entries; i++) {
		/* what type of monster are we reading now? */
		char *sec = iniparser_getsecname(monst_file, i);

		/* add entry to dictionary */
		llist_node tmp = ht_add(monsters_dict, sec, NULL);

		/* allocate monster object */
		tmp->contents = malloc(sizeof(monster));
		new_monst = (monster *)(tmp->contents);

		/* set up monster attributes */
		new_monst->dead = 0;
		new_monst->type = strdup(sec);

		new_monst->symbol    = strdup(iniparser_getstring(monst_file, construct_key(sec, "symbol") , "x"));
		new_monst->evade     = strdup(iniparser_getstring(monst_file, construct_key(sec, "evade")  , "NULL"));
		new_monst->strike    = strdup(iniparser_getstring(monst_file, construct_key(sec, "strike") , "NULL"));
		new_monst->pickup    = strdup(iniparser_getstring(monst_file, construct_key(sec, "pickup") , "NULL"));

		new_monst->cost      = iniparser_getint(monst_file,           construct_key(sec, "cost")   , 1   );
		new_monst->hp        = iniparser_getint(monst_file,           construct_key(sec, "hp")     , 0   );
		new_monst->attack    = iniparser_getint(monst_file,           construct_key(sec, "attack") , 0   );
		new_monst->defense   = iniparser_getint(monst_file,           construct_key(sec, "defense"), 0   );
		new_monst->ai        = iniparser_getint(monst_file,           construct_key(sec, "ai")     , 1   );

		new_monst->is_player = iniparser_getboolean(monst_file,       construct_key(sec, "player") , 0   );
		new_monst->is_object = iniparser_getboolean(monst_file,       construct_key(sec, "object") , 0   );
	}

	return;
}

monster *create_new_monster(char *type)
{
	monster *new;

	monster *stored = (ht_lookup(monsters_dict, type)->contents);
	if(stored == NULL) {
		return NULL;
	}

	new = malloc(sizeof(monster));
	memcpy(new, stored, sizeof(monster));

	return new;
}

static char *construct_key(char *section, char *keyword)
{
	snprintf(buf, sizeof(char)*STRN_SIZE, "%s:%s", section, keyword);

	return buf;
}

void move_monster(monster *mon, int x, int y)
{
	int tile;

	if(mon->is_on_tetromino) {
		tile = board->falling->data[y][x];
		if(tile) { /* still on tetromino */
			if(tile != RED_W && tile != BLUE_W && tile != GREEN_W && tile != YELLOW_W && tile != MAGENTA_W && tile != CYAN_W && tile != WHITE_W) {
				mon->x = x;
				mon->y = y;
			}
			return;
		} else {  /* getting off tetromino */
			mon->is_on_tetromino = 0;

			x = x+board->falling->x;
			y = y+board->falling->y;
		}
	}

	if(!mon->is_on_tetromino) {
		tile = board->data[y][x];

		if(tile) {  /* still on dry land */
			if(tile != RED_W && tile != BLUE_W && tile != GREEN_W && tile != YELLOW_W && tile != MAGENTA_W && tile != CYAN_W && tile != WHITE_W && tile != VOID_W) {
				mon->x = x;
				mon->y = y;
			}
		} else {  /* jumping off */
			//TODO are we getting onto the tetromino ?
			int xdiff = x - board->falling->x;
			int ydiff = y - board->falling->y;
			//int falling = 0;
			if(xdiff < 16 && xdiff >= 0 && ydiff < 16 && ydiff >= 0) { //getting off
				tile = board->falling->data[ydiff][xdiff];

				if(tile) {
					if(tile != RED_W && tile != BLUE_W && tile != GREEN_W && tile != YELLOW_W && tile != MAGENTA_W && tile != CYAN_W && tile != WHITE_W) {
						mon->x = xdiff;
						mon->y = ydiff;
						mon->is_on_tetromino = 1;
						return;
					}
				}
			}

			/* If we got this far, we must have died */
			mon->dead = 1;
		}
	}

	return;
}

int acceptable_move(monster *mon, int direction)
{
	int tile;
	int getting_off = 0;

	int x = mon->x;
	int y = mon->y;

	transform_coord(direction, &x, &y);

	if(mon->is_on_tetromino) {
		if(x < 0 || y < 0 || x > 15 || y > 15)
			return 0;

		tile = board->falling->data[y][x];
		if(tile) { /* still on tetromino */
			if(tile != RED_W && tile != BLUE_W && tile != GREEN_W && tile != YELLOW_W && tile != MAGENTA_W && tile != CYAN_W && tile != WHITE_W) {
				return 1;
			}
			return 0;
		} else {  /* getting off tetromino */
			getting_off = 1;
		}
	}

	if(!mon->is_on_tetromino || getting_off) {
		tile = board->data[y][x];

		if(tile) {  /* still on dry land */
			if(tile != RED_W && tile != BLUE_W && tile != GREEN_W && tile != YELLOW_W && tile != MAGENTA_W && tile != CYAN_W && tile != WHITE_W && tile != VOID_W) {
				return 1;
			}
		} else {  /* jumping off */
			//TODO are we getting onto the tetromino ?
			int xdiff = x - board->falling->x;
			int ydiff = y - board->falling->y;
			//int falling = 0;
			if(xdiff < 16 && xdiff >= 0 && ydiff < 16 && ydiff >= 0) { //getting off
				tile = board->falling->data[ydiff][xdiff];

				if(tile) {
					if(tile != RED_W && tile != BLUE_W && tile != GREEN_W && tile != YELLOW_W && tile != MAGENTA_W && tile != CYAN_W && tile != WHITE_W) {
						return 1;
					}
				}
			}

			/* If we got this far, we must have died */
			return 0;
		}
	}

	return 0;
}

void transform_coord(int direction, int *x, int *y)
{
	switch(direction) {
		case UP:    (*y)--; break;
		case LEFT:  (*x)--; break;
		case DOWN:  (*y)++; break;
		case RIGHT: (*x)++; break;

		case UPLEFT:    (*y)--; (*x)--; break;
		case DOWNLEFT:  (*y)++; (*x)--; break;
		case DOWNRIGHT: (*y)++; (*x)++; break;
		case UPRIGHT:   (*y)--; (*x)++; break;
	}

	return;
}

void ai(monster *mon)
{
	int x = mon->x, y = mon->y;
	int dir;

	switch(mon->ai) {
		case RANDWALK:
			dir = rand() % 8;
			if(acceptable_move(mon, dir)) {
					transform_coord(dir, &x, &y);
					move_monster(mon, x, y);
			}
			break;
		case RANGE: break;
		case MELEE: break;
	}

	return;
}

void bump_attack(monster *attacker, monster *defender)
{
	if(defender->is_object) {
		if(!attacker->is_player)
			return;

		attacker->hp += defender->hp;
		attacker->defense += defender->defense;
		attacker->attack  += defender->attack;

		/* pick up, then kill  */
	}

	int attack = attacker->attack;
	int defense = defender->defense;

	double hitrate = (double)attack / (double)(attack + defense);
	double roll_to_hit = (double)rand() / (double)RAND_MAX;

	if(roll_to_hit > hitrate) {
		/* MISS MESSAGE */
		return;
	}

	double damage = (double)rand() / (double)RAND_MAX;

	defender->hp -= (int)(damage * (double)attack);

	/* HIT MESSAGE */

	if(defender->hp <= 0) {
		defender->dead = 1;
		/* KILL MESSAGE */
	}

	return;
}
