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

void move_tetromino_down(level *lev)
{
	int x, y;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+board->falling->x >= board->width || y+board->falling->y > board->height)
			continue;

		if(board->falling->data[y][x] && board->data[y+board->falling->y+4][x+board->falling->x]) {
			/* place tetromino */
			place_tetromino(board);
			return;
		}
	}

	board->falling->y += 4;
	return;
}

void move_tetromino_right(level *lev)
{
	int x, y;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+board->falling->x >= board->width || y+board->falling->y > board->height)
			continue;

		if(board->falling->data[y][x] && board->data[y+board->falling->y][x+board->falling->x+4]) {
			/* place tetromino */
			return;
		}
	}

	board->falling->x += 4;
	return;
}
void move_tetromino_left(level *lev)
{
	int x, y;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+board->falling->x >= board->width || y+board->falling->y > board->height)
			continue;

		if(board->falling->data[y][x] && board->data[y+board->falling->y][x+board->falling->x-4]) {
			/* place tetromino */
			return;
		}
	}

	board->falling->x -= 4;
	return;
}

void place_tetromino(level *lev)
{
	int y, x;

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+board->falling->x >= board->width || y+board->falling->y > board->height)
			continue;

		if(board->falling->data[y][x])
			board->data[y+board->falling->y][x+board->falling->x] = board->falling->data[y][x];
	}

	for_each(x, board->monsters) {
		monster *tmp = ((monster *)al_retrieve(x, board->monsters));
		if(tmp->is_on_tetromino) {
			tmp->is_on_tetromino = 0;
			tmp->x = tmp->x + board->falling->x;
			tmp->y = tmp->y + board->falling->y;
		}
	}

	if(player->is_on_tetromino) {
		player->is_on_tetromino = 0;
		player->x = player->x + board->falling->x;
		player->y = player->y + board->falling->y;
	}

	free(board->falling);


	gen_new_tetromino();

	eliminate_rows();
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

	for_each(x, board->monsters) {
		monster *tmp = (monster *)al_retrieve(x, board->monsters);
		if(tmp->is_on_tetromino)
			rotate_coords(&(tmp->x), &(tmp->y), board->falling->n);
	}

	return;
}

void eliminate_rows()
{
	int x, y;
	int k;
	int full;

	for(y=board->height-2; y>=1; y--) {
		full = 1;

		for(x=1; x<board->width-1; x++) {
			if(board->data[y][x] == VOID) {
				full = 0;
				break;
			}
		}

		if(!full)
			continue;

		for_each(k, board->monsters) {
			monster *tmp = ((monster *)al_retrieve(k, board->monsters));
			if(tmp->is_on_tetromino)
				continue;

			if(tmp->y == y)
				tmp->dead = 1;

			if(tmp->y < y)
				tmp->y++;
		}

		/* move down */
		for(k=y; k>=2; k--)
		for(x=1; x<board->width-1; x++)
			board->data[k][x] = board->data[k-1][x];

		y++;
	}

	return;
}



void gen_new_tetromino()
{
	int x, y;
	int i;
	int color=0;
	monster *tmp;
	board->falling = new_tetromino(rand() % 7);
	board->falling->x = 13;
	board->falling->y = 1;

	int num_walls = rand() % 15;
	for(i=0; i<num_walls; i++) {
		place_on_tetromino(&x, &y);

		switch(board->falling->color) {
			case RED: color = RED_W;
				break;
			case GREEN: color = GREEN_W;
				break;
			case YELLOW: color = YELLOW_W;
				break;
			case BLUE: color = BLUE_W;
				break;
			case MAGENTA: color = MAGENTA_W;
				break;
			case CYAN: color = CYAN_W;
				break;
			case WHITE: color = WHITE_W;
				break;
		}

		board->falling->data[y][x] = color;
	}

	/* TODO
	 * generate better walls, objects
	 */
	int money = tetromino_allowance;

//	while(money >= 2) {
	for(i=0; i<5; i++) {
//		switch(rand()%3) {
//			case 0:
		tmp = create_new_monster("lab-rat");

		al_insert(tmp, al_first(board->monsters), board->monsters);
		money -= tmp->cost;

		place_on_tetromino(&(tmp->x), &(tmp->y));

		tmp->is_on_tetromino = 1;
//				break;
//			case 1:
//				al_insert(create_new_monster("chimp"), al_first(board->monsters), board->monsters);
//				money -= ((monster *)al_retrieve(al_first(board->monsters), board->monsters))->cost;
//				break;
//			case 2:
//				al_insert(create_new_monster("bear"), al_first(board->monsters), board->monsters);
//				money -= ((monster *)al_retrieve(al_first(board->monsters), board->monsters))->cost;
//				break;
//		}
	}

	return;
}

void place_on_tetromino(int *x, int *y)
{
	int i, a, b, cont;

	while(1) {
		a = rand()%16;
		b = rand()%16;
		cont = 0;

		switch(board->falling->data[b][a]) {
			case RED: case WHITE: case MAGENTA:
			case BLUE: case GREEN: case YELLOW:
			case CYAN:
				break;
			default:
				continue;
		}

		for_each(i, board->monsters) {
			monster *tmp = ((monster *)al_retrieve(i, board->monsters));
			if(tmp->is_on_tetromino)
				if(tmp->x == a && tmp->y == b)
					cont = 1;
		}

		if(cont) continue;

		*x = a;
		*y = b;

		return;
	}

	return;
}
