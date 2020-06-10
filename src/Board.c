#include <stdio.h>
#include <time.h>
#include <conio.h>
#include "Board.h"

void segment_swap(game_board* board, uint pos1, uint pos2)
{
	struct game_board_segment seg_cpy = board->board_segments[pos1];
	seg_cpy.col = board->board_segments[pos2].col;
	seg_cpy.row = board->board_segments[pos2].row;

	board->board_segments[pos2].col = board->board_segments[pos1].col;
	board->board_segments[pos2].row = board->board_segments[pos1].row;

	board->board_segments[pos1] = board->board_segments[pos2];
	
	board->board_segments[pos2] = seg_cpy;
}

void generate_board(game_board* board)
{
	srand(time(0));

	for (size_t i = 0; i < WIDTH; i++)
	{
		for (size_t j = 0; j < HEIGHT; j++)
		{
			if (i == WIDTH - 1 && j == HEIGHT - 1)
			{
				struct game_board_segment* current_segment = &board->board_segments[(i * 4) + j];
				current_segment->col = i;
				current_segment->row = j;
				current_segment->identity = 0;
				current_segment->is_spacer = 1;
				current_segment->currently_selected = 0;
			}
			else
			{
				struct game_board_segment* current_segment = &board->board_segments[(i * 4) + j];

				current_segment->col = i;
				current_segment->row = j;
				current_segment->identity = ((i * 4) + j) + 1;
				current_segment->is_spacer = 0;
				current_segment->currently_selected = 0;
			}

		}
	}

	board->board_size = GRIDLENGTH + 1;
	board->col_max = WIDTH;
	board->row_max = HEIGHT;
}

void randomize_board(game_board* board)
{
	for (uint i = 0; i < GRIDLENGTH; i++)
	{
		int value = (int)(rand() % 15);
		segment_swap(board, i, value);
	}
}

/* This is a poor implmentation that should be removed later on. */
int number_order[GRIDLENGTH] = { 0 };
uint is_state_old(game_board* board)
{
	uint is_changed = 0;

	/* Starting check -- should update in this case. */
	if (number_order[0] < 1)
	{
		for (size_t i = 0; i < GRIDLENGTH; i++)
		{
			number_order[i] = board->board_segments[i].identity;
		}
		return 1;
	}

	for (size_t i = 0; i < GRIDLENGTH; i++)
	{
		if (number_order[i] != board->board_segments[i].identity)
		{
			is_changed = 1;
		}
	}

	/* Update the stored numbers at the end of this loop no matter what. */
	for (size_t i = 0; i < GRIDLENGTH; i++)
	{
		number_order[i] = board->board_segments[i].identity;
	}

	return is_changed;
}

void render_board(game_board* board)
{
	/* If this is true there is no need for us to update the display. */
	if (!is_state_old(board))
		return;

	/* Pre clear the screen, to prevent a frame skip. */

	system("cls");

	for (uint i = 0; i < WIDTH; i++)
	{
		for (uint j = 0; j < HEIGHT; j++)
		{
			if (i == 3 && j == 3)
			{
				break;
			}
			printf("  %2.1u  ", board->board_segments[((i * 4) + j)].identity);
		}
		printf("\n");
	}

}

void cleanup_board(game_board* board)
{

}