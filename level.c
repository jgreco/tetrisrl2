#include "level.h"
#include "arraylist.h"
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "util.h"

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

void move_tetromino_down(level *lev)
{
	int x, y;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+lev->falling->x >= lev->width || y+lev->falling->y > lev->height)
			continue;

		if(lev->falling->data[y][x] && lev->data[y+lev->falling->y+1][x+lev->falling->x]) {
			/* place tetromino */
			place_tetromino(lev);
			return;
		}
	}

	lev->falling->y += 1;
	return;
}

void move_tetromino_right(level *lev)
{
	int x, y;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+lev->falling->x >= lev->width || y+lev->falling->y > lev->height)
			continue;

		if(lev->falling->data[y][x] && lev->data[y+lev->falling->y][x+lev->falling->x+4]) {
			/* place tetromino */
			return;
		}
	}

	lev->falling->x += 4;
	return;
}
void move_tetromino_left(level *lev)
{
	int x, y;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+lev->falling->x >= lev->width || y+lev->falling->y > lev->height)
			continue;

		if(lev->falling->data[y][x] && lev->data[y+lev->falling->y][x+lev->falling->x-4]) {
			/* place tetromino */
			return;
		}
	}

	lev->falling->x -= 4;
	return;
}

void place_tetromino(level *lev)
{
	int y, x;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+lev->falling->x >= lev->width || y+lev->falling->y > lev->height)
			continue;

		if(lev->falling->data[y][x])
			lev->data[y+lev->falling->y][x+lev->falling->x] = lev->falling->data[y][x];
	}

	for_each(x, board->monsters) {
		monster *tmp = ((monster *)al_retrieve(x, board->monsters));
		if(tmp->is_on_tetromino) {
			tmp->is_on_tetromino = 0;
			tmp->x = tmp->x + lev->falling->x;
			tmp->y = tmp->y + lev->falling->y;
		}
	}

	if(player->is_on_tetromino) {
		player->is_on_tetromino = 0;
		player->x = player->x + lev->falling->x;
		player->y = player->y + lev->falling->y;
	}

	free(lev->falling);

	gen_new_tetromino();

	return;
}

void rotate_tetromino()
{
	int x, y;

	tetromino *new = copy_rotated_tetromino(board->falling);

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+board->falling->x >= board->width || y+board->falling->y > board->height)
			continue;

		if(new->data[y][x] && board->data[y+board->falling->y][x+board->falling->x]) {
			free(new);
			return;
		}
	}

	memcpy(board->falling->data, new->data, sizeof(int)*16*16);

	free(new);

	if(player->is_on_tetromino)
		rotate_coords(&(player->x), &(player->y), board->falling->n);

	return;
}

void gen_new_tetromino()
{
	board->falling = new_tetromino(rand() % 7);
	board->falling->x = 13;
	board->falling->y = 1;

	/* TODO
	 *
	 * generate walls, objects, and monsters here.
	 */

	return;
}
