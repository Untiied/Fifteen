#ifndef BOARD_H
#define BOARD_H

#define WIDTH 4
#define HEIGHT 4
#define uint unsigned int

/* The amount of objects in the grid. */
#define GRIDLENGTH (WIDTH * HEIGHT) - 1

struct game_board_segment
{
	uint col;
	uint row;

	/* The number that this row segment actually has. */
	uint identity;

	int is_spacer;

	int currently_selected;
};

typedef struct
{
	/* Subtract one from the total value to allocated for the missing number in the lower right. */
	struct game_board_segment board_segments[GRIDLENGTH + 1];

	uint board_size;
	
	uint col_max;
	
	uint row_max;
	/* We need some type of rendering command for command line.*/


} game_board;


/* 
	This will swap the two game pieces. They way we do it now may seem overkill, 
	but when each object has rendering data it is how we are going to want it set up.
*/
void segment_swap(game_board* board, uint pos1, uint pos2);

/* This will generate and populate the Fifteen Puzzle board. */
void generate_board(game_board* board);

/* This will take all the pieces in the board and do a flip with a number for its position. */
void randomize_board(game_board* board);

/* This will render the board into the command line window. This will allow for easier debugging for the actual rendering window.*/
void render_board(game_board* board);

/* This should remove all the board items. */
void cleanup_board(game_board* board);

#endif // !BOARD_H
