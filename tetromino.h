#ifndef TETROMINO_H_
#define TETROMINO_H_


enum TETROMINO_TYPES { I=0, J=1, L=2, O=3, S=4, T=5, Z=6 };

typedef struct tetromino_struct {
	int x, y;  /* absolute coords */
	int n;  /* size of grid (used in rotation) */
	int color;

	int data[16][16];
} tetromino;

tetromino *new_tetromino(int type);
tetromino *new_blank_tetromino();
tetromino *copy_rotated_tetromino(tetromino *piece);

#endif
