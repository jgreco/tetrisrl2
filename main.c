#include <ncurses.h>

#include "level.h"
#include "tetromino.h"
#include "monster.h"
#include "arraylist.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "defines.h"

int ninterface();
void *drop_tetromino();
int gameloop();
void draw_screen();

enum modes { ROGUE, TETRIS };
int input_mode;

int offset_y;

pthread_mutex_t display_mutex;

enum COLOR_PAIR_NAMES { BLACK_P, RED_P, GREEN_P, YELLOW_P, BLUE_P, MAGENTA_P, CYAN_P, WHITE_P };

int main()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	start_color();
	use_default_colors();
	assume_default_colors(COLOR_WHITE, COLOR_BLACK);

	init_pair(BLACK_P  , COLOR_BLACK, COLOR_WHITE);
	init_pair(RED_P    , COLOR_RED, COLOR_BLACK);
	init_pair(GREEN_P  , COLOR_GREEN, COLOR_BLACK);
	init_pair(YELLOW_P , COLOR_YELLOW, COLOR_BLACK);
	init_pair(BLUE_P   , COLOR_BLUE, COLOR_BLACK);
	init_pair(MAGENTA_P, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(CYAN_P   , COLOR_CYAN, COLOR_BLACK);
	init_pair(WHITE_P  , COLOR_WHITE, COLOR_BLACK);

	srand(time(NULL));

	/* STARTING STUFF */

	/* INIT GAME OBJECTS */
	init_monsters();
	board = make_level();
	board->monsters = al_makenull(NULL);
	gen_new_tetromino();

	player = create_new_monster("player");
	player->is_on_tetromino = 1;

	player->x = 5;
	player->y = 5;


	/* GAME LOOP */
	gameloop();

	endwin();
	return 0;
}

int gameloop()
{
	//pthread_t ai_loop;
	pthread_t drop_loop;

	pthread_mutex_init(&display_mutex, NULL);

	pthread_create(&drop_loop, NULL, drop_tetromino, NULL);
	//pthread_create(&ai_loop, NULL, ai_run, NULL);

	ninterface();

	return 0;
}

void *drop_tetromino()
{
	while(1) {
		usleep(250000);
		move_tetromino_down(board);
		draw_screen();
	}

	return NULL;
}

/* handle input from player */
int ninterface()
{
	draw_screen();
	int key = 0;
	while(key != 'Q' && !player->dead) {
		key = getch();

		if(input_mode == ROGUE) {
			switch(key) {
				case ' ':  /* switch to tetris mode */
					input_mode = TETRIS;
					continue;
					break;

				case 'h': move_monster(player, player->x-1, player->y);   break;
				case 'j': move_monster(player, player->x, player->y+1);   break;
				case 'k': move_monster(player, player->x, player->y-1);   break;
				case 'l': move_monster(player, player->x+1, player->y);   break;

				case 'y': move_monster(player, player->x-1, player->y-1); break;
				case 'u': move_monster(player, player->x+1, player->y-1); break;
				case 'b': move_monster(player, player->x-1, player->y+1); break;
				case 'n': move_monster(player, player->x+1, player->y+1); break;
			}
		} else { /* TETRIS */
			switch(key) {
				case ' ':  /* switch to rogue mode */
					input_mode = ROGUE;
					continue;
					break;

				case 'k':
					rotate_tetromino();
					break;
				case 'l':
					move_tetromino_right(board);
					break;
				case 'h':
					move_tetromino_left(board);
					break;
			}

		}

		draw_screen();
	}

	return 0;
}

void draw_screen()
{
	pthread_mutex_lock(&display_mutex);

	int x, y;
	int color = 0;
	int tmp;
	void *dumb = NULL;
	erase();

	for(y=0; y<LINES; y++)
	for(x=0; x<COLS; x++) {
		if(x >= board->width || y+offset_y < 0 || y+offset_y >= board->height)
			continue;

		tmp = board->data[y][x];

		switch(tmp) {
			case VOID:
				color = BLACK_P;
				break;
			case RED: case RED_W:
				color = RED_P;
				break;
			case GREEN: case GREEN_W:
				color = GREEN_P;
				break;
			case YELLOW: case YELLOW_W:
				color = YELLOW_P;
				break;
			case BLUE: case BLUE_W:
				color = BLUE_P;
				break;
			case MAGENTA: case MAGENTA_W:
				color = MAGENTA_P;
				break;
			case CYAN: case CYAN_W:
				color = CYAN_P;
				break;
			case WHITE: case WHITE_W:
				color = WHITE_P;
				break;
		}

		if(tmp == VOID_W || tmp == RED_W || tmp == GREEN_W || tmp == YELLOW_W || tmp == BLUE_W || tmp == MAGENTA_W || tmp == CYAN_W || tmp == WHITE_W)
			mvprintw(y, x, "#", x);
		else
			mvprintw(y, x, " ");


		if(tmp != VOID && tmp != VOID_W)
			mvchgat(y, x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
	}

	for(y=0; y<16; y++)
	for(x=0; x<16; x++) {
		if(x+board->falling->x >= board->width || y+board->falling->y >= board->height)
			continue;

		tmp = board->falling->data[y][x];

		switch(tmp) {
			case VOID:
				color = BLACK_P;
				break;
			case RED: case RED_W:
				color = RED_P;
				break;
			case GREEN: case GREEN_W:
				color = GREEN_P;
				break;
			case YELLOW: case YELLOW_W:
				color = YELLOW_P;
				break;
			case BLUE: case BLUE_W:
				color = BLUE_P;
				break;
			case MAGENTA: case MAGENTA_W:
				color = MAGENTA_P;
				break;
			case CYAN: case CYAN_W:
				color = CYAN_P;
				break;
			case WHITE: case WHITE_W:
				color = WHITE_P;
				break;
		}

		if(tmp == VOID_W || tmp == RED_W || tmp == GREEN_W || tmp == YELLOW_W || tmp == BLUE_W || tmp == MAGENTA_W || tmp == CYAN_W || tmp == WHITE_W)
			mvprintw(y+board->falling->y, x+board->falling->x, "#", x);
		else if(tmp != VOID)
			mvprintw(y+board->falling->y, x+board->falling->x, " ");


		if(tmp != VOID && tmp != VOID_W)
			mvchgat(y+board->falling->y, x+board->falling->x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
	}

	if(player->is_on_tetromino) {
		color = board->falling->data[player->y][player->x];

		mvprintw(player->y+board->falling->y, player->x+board->falling->x, "@");
		mvchgat( player->y+board->falling->y, player->x+board->falling->x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
	}
	else {
		color = board->data[player->y][player->x];
		mvprintw(player->y, player->x, "@");
		mvchgat( player->y, player->x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
	}

	refresh();

	pthread_mutex_unlock(&display_mutex);

	return;
}
