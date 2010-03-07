#include <ncurses.h>

#include "level.h"
#include "tetromino.h"
#include "monster.h"
#include "arraylist.h"

int ninterface();
int gameloop();
void draw_screen();

enum modes { ROGUE, TETRIS };
int input_mode;

level *board;
arraylist monsters;
monster *player;

int offset_y;

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

	/* STARTING STUFF */

	/* INIT GAME OBJECTS */
	monsters = al_makenull(NULL);
	board = make_level();

	/* GAME LOOP */
	//gameloop();
	//
	draw_screen();
	getch();


	endwin();
	return 0;
}

int gameloop()
{
	/*TODO
	//spawn three pthreads:
	//  player I/O
	//  monster AI
	//  tetris movement */
	return 0;
}

/* handle input from player */
int ninterface()
{
	int key = 0;
	while(key != 'Q') {
		key = getch();

		if(input_mode == ROGUE) {
			switch(key) {
				case ' ':
					input_mode = TETRIS;
					continue;
					break;
			}
		} else { /* TETRIS */
			switch(key) {
				case ' ':
					input_mode = ROGUE;
					continue;
					break;
			}
		}
	}

	return 0;
}

void draw_screen()
{
	int x, y;
	int color = 0;
	int tmp;
	void *dumb = NULL;
//	erase();

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


		if(tmp != VOID & tmp != VOID_W)
			mvchgat(y, x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
	}
}
