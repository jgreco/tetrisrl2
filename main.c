#include <ncurses.h>

#include "level.h"
#include "tetromino.h"
#include "monster.h"
#include "arraylist.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "defines.h"
#include "debugging.h"
#include <locale.h>

int ninterface();
void *drop_tetromino();
void *ai_run();
int gameloop();
void draw_screen();

enum modes { ROGUE, TETRIS };
int input_mode;

int offset_y;

pthread_mutex_t display_mutex;

enum COLOR_PAIR_NAMES { BLACK_P, RED_P, GREEN_P, YELLOW_P, BLUE_P, MAGENTA_P, CYAN_P, WHITE_P };

int main()
{
	if(!setlocale(LC_CTYPE, "en_US.UTF-8")) {
		return 1;
	}

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
	tetromino_allowance = 5;
	stop_tetrominos = 0;
	lines_cleared = 0;
	score = 0;
	logging = fopen("log", "w");

	/* INIT GAME OBJECTS */
	init_monsters();
	board = make_level();
	board->monsters = al_makenull(NULL);
	gen_new_tetromino();

	messages = al_makenull(NULL);

	player = create_new_monster("player");
	player->is_on_tetromino = 1;
	place_on_tetromino(&(player->x), &(player->y));

	al_insert(player, al_first(board->monsters), board->monsters);

	/* INTRO */
	mvprintw(0, 0, "Velcome back comrade! I zee you have fully recovered from our last experiment.\n\n" \
"Now ve can get back to doing some science!\n\n" \
"If you look around you vill zee that ze laboratory is much larger this time,\n" \
"zis is appropriate for ze complexity of ze experiment has increased as well!\n" \
"You are standing on an oddly shaped platform zat is moving slowly but surely\n" \
"toward ze other side of ze laboratory.  In your possession you will fine a\n" \
"remote control to control ze movement and rotation of ze platform.\n" \
"You may activate ze remote with [SPACE], and deactivate it ze same way.\n\n" \
"You may position ze platforms as you wish, though be warned zey are rigged\n" \
"to detonate if too many of zem align in a row!  It is most unhealthy to be\n" \
"ontop of zem when zey do so!  Try to get back to ze top if you can.");

	getch();

	mvprintw(17, 0, "Oh, and comrade, I almost forgot to tell you:  it seems through a completely\n" \
"bad stroke of luck that more of my laboratory animals have once again escaped\n" \
"Be warned that some of zem are far larger than you are used to dealing with.\n" \
"I may be able to find some supplies to send done on ze platforms for you so\n" \
"zat you can deal with zem.  Good luck comrade!  And do try not to fall off!\n");
	getch();


	/* GAME LOOP */
	gameloop();

	endwin();
	return 0;
}

int gameloop()
{
	pthread_t ai_loop;
	pthread_t drop_loop;

	pthread_mutex_init(&display_mutex, NULL);

	pthread_create(&drop_loop, NULL, drop_tetromino, NULL);
	pthread_create(&ai_loop, NULL, ai_run, NULL);

	ninterface();

	return 0;
}

void *drop_tetromino()
{
	while(1) {
		if(stop_tetrominos)
			return NULL;
		usleep(1250000);
		move_tetromino_down(board);
		draw_screen();
	}

	return NULL;
}

void *ai_run()
{
	int i;

	while(1) {
		for_each(i, board->monsters) {
			monster *tmp = (monster *)al_retrieve(i, board->monsters);
			if(tmp->dead)
				continue;

			if(tmp->is_player || tmp->is_object) {
				continue;
			}

			ai(tmp);
		}

		draw_screen();
		usleep(1250000);
	}

	return NULL;
}

/* handle input from player */
int ninterface()
{
	draw_screen();
	int key = 0;
	while(key != 'Q' && !player->dead) {
		flushinp();
		if(stop_tetrominos && player->y <= 5) {
			endwin();
			printf("Zat is good enough comrade!  I am very pleased with these results!  I'll let\nyou get some rest, zen we can begin a new experiment in ze morning!\n"); fflush(stdout);
			exit(EXIT_SUCCESS);
		}

		key = getch();

		if(input_mode == ROGUE) {
			switch(key) {
				case ' ':  /* switch to tetris mode */
					input_mode = TETRIS;
					continue;
					break;

				case 'h': case KEY_LEFT: case 'a': move_monster(player, player->x-1, player->y);   break;
				case 'j': case KEY_DOWN: case 's': move_monster(player, player->x, player->y+1);   break;
				case 'k': case KEY_UP:   case 'w': move_monster(player, player->x, player->y-1);   break;
				case 'l': case KEY_RIGHT: case 'd': move_monster(player, player->x+1, player->y);   break;

				case 'y': case 'q': move_monster(player, player->x-1, player->y-1); break;
				case 'u': case 'e': move_monster(player, player->x+1, player->y-1); break;
				case 'b': case 'z': move_monster(player, player->x-1, player->y+1); break;
				case 'n': case 'c': move_monster(player, player->x+1, player->y+1); break;
			}
		} else { /* TETRIS */
			switch(key) {
				case ' ':  /* switch to rogue mode */
					input_mode = ROGUE;
					continue;
					break;

				case 'k': case 'w': case KEY_UP:
					rotate_tetromino();
					break;
				case 'l': case 'd': case KEY_RIGHT:
					move_tetromino_right(board);
					break;
				case 'h': case 'a': case KEY_LEFT:
					move_tetromino_left(board);
					break;
			}

		}

		draw_screen();
		if(input_mode == ROGUE)
			usleep(100000);
	}
	endwin();
	printf("I am very disappointed with you comrade...\n"); fflush(stdout);
	exit(EXIT_SUCCESS);

	return 0;
}

void draw_screen()
{
	pthread_mutex_lock(&display_mutex);

	int x, y;
	int i;
	int color = 0;
	int tmp;
	void *dumb = NULL;
	erase();

	if(LINES < board->height) {
		if(input_mode == TETRIS || player->is_on_tetromino) {
			offset_y = board->falling->y;
		} else {
			offset_y = player->y - (LINES / 2);
			if(offset_y < 0)
				offset_y = 0;
		}

		if(offset_y > board->height - LINES)
			offset_y = board->height-LINES;
	}

	for(y=0; y<LINES; y++)
	for(x=0; x<COLS; x++) {
		if(x >= board->width || y+offset_y < 0 || y+offset_y >= board->height)
			continue;

		tmp = board->data[y+offset_y][x];

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
			mvprintw(y+board->falling->y - offset_y, x+board->falling->x, "#", x);
		else if(tmp != VOID)
			mvprintw(y+board->falling->y - offset_y, x+board->falling->x, " ");


		if(tmp != VOID && tmp != VOID_W)
			mvchgat(y+board->falling->y - offset_y, x+board->falling->x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
	}

	for_each(x, board->monsters) {
		monster *tmp = (monster *)al_retrieve(x, board->monsters);

		if(tmp->dead)
			continue;

		if(tmp->is_on_tetromino) {
			color = board->falling->data[tmp->y][tmp->x];
			mvprintw(tmp->y+board->falling->y - offset_y, tmp->x+board->falling->x, "%s", tmp->symbol);
			mvchgat( tmp->y+board->falling->y - offset_y, tmp->x+board->falling->x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
		} else {
			color = board->data[tmp->y][tmp->x];
			mvprintw(tmp->y - offset_y, tmp->x, "%s", tmp->symbol);
			mvchgat( tmp->y - offset_y, tmp->x, 1, COLOR_PAIR(color)|A_BOLD|A_REVERSE, color, dumb);
		}
	}

	if(board->height > LINES) {
		double percet = (double)LINES / (double)board->height;
		double percet2 = (double)offset_y / (double)(board->height - LINES);

		int bar = percet * (double)LINES;

		int down = (LINES - bar) * percet2;


		for(i=0; i<LINES; i++) {
#ifdef NO_UNICODE
			mvprintw(i, 42, "##");
#else
			mvprintw(i, 42, "▒▒");
#endif
			mvchgat(i, 42, 2, COLOR_PAIR(WHITE)|A_REVERSE, WHITE, dumb);
		}
		for(i=down; i<LINES && (i-down) < bar; i++) {
			mvprintw(i, 42, "  ");
			mvchgat(i, 42, 2, COLOR_PAIR(WHITE)|A_BOLD|A_REVERSE, WHITE, dumb);
		}

	}

	if(input_mode == TETRIS)
		mvprintw(7, 50, "TETRIS MODE");
	else
		mvprintw(7, 50, "ROGUE MODE");

	mvprintw(8, 50, "   (space to toggle)");

	mvprintw(10, 50, "SCORE:");
	mvprintw(11, 50, "   lines cleared: %d", lines_cleared/4);

	mvprintw(14, 50, "STATS:");
	mvprintw(15, 50, "   HP: %d", player->hp);
	mvprintw(17, 50, "   Attack: %d", player->attack);
	mvprintw(18, 50, "   Defense: %d", player->defense);

	for(i=0; i<LINES-20; i++) {
		char *mes = al_retrieve(i, messages);
		if(mes != NULL)
			mvprintw( 20+i, 50,"%s", mes);
	}

	refresh();

	pthread_mutex_unlock(&display_mutex);

	return;
}
