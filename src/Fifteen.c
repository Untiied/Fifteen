#include <stdio.h>
#include "Board.h"

#include "Window.h"
#include "Rendering.h"
#include <glad/glad.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/cam.h>
#include <cglm/affine.h>
struct window_size  test = { 800, 800 };
window_payload window = { 800, 800, "Fifteen Puzzle" };

int has_won(game_board* board)
{
	int last_number = 0;
	for (size_t i = 0; i < board->board_size; i++)
	{
		struct game_board_segment* segment = &board->board_segments[i];

		if (segment->is_spacer)
			continue;

		if (segment->identity > last_number)
		{
			last_number = segment->identity;
			continue;
		}
		else
			return 0;
	}

	return 1;
}

int get_segment_at_position(game_board* board, mouse_position* first)
{
	/* Figure out the row, and col where the click was, approximate that with the closed segment. */
	precomputed_renderable_target_transforms transform = calculate_rendering_grid(board, test);
	const float x_max_box_size = transform.width + EDGEGUARD * 2;
	const float y_max_box_size = transform.height + EDGEGUARD * 2;

	float last_pos = 0.0;
	int col = 0;
	while (true)
	{
		if (last_pos > 10000)
			break;

		float future_pos = last_pos + x_max_box_size;
		if (first->x > last_pos && first->x < future_pos)
		{
			break;
		};
		last_pos += x_max_box_size;
		col++;
	}

	last_pos = 0.0;
	int row = 0;
	while (true)
	{
		if (last_pos > 10000)
			break;

		float future_pos = last_pos + y_max_box_size;
		if (first->y > last_pos && first->y < future_pos)
		{
			break;
		}
		last_pos += y_max_box_size;
		row++;
	}

	printf("Position: [%d, %d]\n", col, row);
	printf("Mouse: [%f, %f]\n", first->x, first->y);

	for (size_t i = 0; i < board->board_size; i++)
	{
		struct game_board_segment* segment = &board->board_segments[i];
		if (segment->col == col)
			if (segment->row == row)
				return i;
	}

	return 0;
}

int is_first_selection = 1;
int is_ready_for_swap = 0;
int keep_progress = 0;
mouse_position mouse_pos_1;
mouse_position mouse_pos_2;

void slide_select_two_points(game_board* win)
{
	if (get_mouse_down(LEFT) == 1)
	{
		mouse_pos_1 = *get_mouse_position(win);
		int seg_1 = get_segment_at_position(win, &mouse_pos_1);
		win->board_segments[seg_1].currently_selected = 1;
		keep_progress = 1;
	}

	if (get_mouse_up(LEFT) == 1)
	{
		is_first_selection = 1;
		is_ready_for_swap = 1;
		mouse_pos_2 = *get_mouse_position(win);
	}

	int been_swapped = 0;
	if (is_ready_for_swap == 1)
	{
		is_ready_for_swap = 0;
		int seg_1 = get_segment_at_position(win, &mouse_pos_1);
		int seg_2 = get_segment_at_position(win, &mouse_pos_2);

		if (abs(win->board_segments[seg_1].col - win->board_segments[seg_2].col) < 2 && abs(win->board_segments[seg_1].row - win->board_segments[seg_2].row) < 2)
		{
			if (win->board_segments[seg_1].is_spacer || win->board_segments[seg_2].is_spacer)
			{
				segment_swap(win, seg_1, seg_2);
				been_swapped = 1;
			}
		}

		printf("Position 1: %f, %f \n Position 2: %f, %f \n", mouse_pos_1.x, mouse_pos_1.y, mouse_pos_2.x, mouse_pos_2.y);
		if (been_swapped)
			win->board_segments[seg_2].currently_selected = 0;
		else
			win->board_segments[seg_1].currently_selected = 0;
	}
}

void click_two_points(game_board* win)
{
	/* This is a terrible implmentation because this will always run on the main thread... This is TEMP! */
	//mouse_position mouse_pos_1 = get_mouse_position(win);
	//set_mouse_state(0, 0);

	if (is_first_selection == 1)
	{
		if (get_mouse_down(LEFT) == 1)
		{
			is_first_selection = 0;
			mouse_pos_1 = *get_mouse_position(win);
			int seg_1 = get_segment_at_position(win, &mouse_pos_1);
			win->board_segments[seg_1].currently_selected = 1;
		}
	}
	else {
		if (get_mouse_down(LEFT) == 1)
		{
			is_first_selection = 1;
			is_ready_for_swap = 1;
			mouse_pos_2 = *get_mouse_position(win);
		}
	}

	int been_swapped = 0;
	if (is_ready_for_swap == 1)
	{
		is_ready_for_swap = 0;
		int seg_1 = get_segment_at_position(win, &mouse_pos_1);
		int seg_2 = get_segment_at_position(win, &mouse_pos_2);

		if (abs(win->board_segments[seg_1].col - win->board_segments[seg_2].col) < 2 && abs(win->board_segments[seg_1].row - win->board_segments[seg_2].row) < 2)
		{
			if (win->board_segments[seg_1].is_spacer || win->board_segments[seg_2].is_spacer)
			{
				segment_swap(win, seg_1, seg_2);
				been_swapped = 1;
			}
		}

		printf("Position 1: %f, %f \n Position 2: %f, %f \n", mouse_pos_1.x, mouse_pos_1.y, mouse_pos_2.x, mouse_pos_2.y);
		if(been_swapped)
			win->board_segments[seg_2].currently_selected = 0;
		else
			win->board_segments[seg_1].currently_selected = 0;
	}
}

int main()
{
	game_board board;
	
	/* The only thing that will remain true in this is a window reference. Don't base any calculations off this. */
	create_window(&window);

	generate_board(&board);
	randomize_board(&board);


	loaded_texture textures[GRIDLENGTH] = {0};
	for (size_t i = 1; i < GRIDLENGTH + 1; i++)
	{
		char number[sizeof(i) / sizeof(size_t)];

		/* This is 4 because we have the .png and null terminator. */
		char* path = malloc((int)strlen("../assets/") + (int)strlen(number) + 4);
		sprintf(path, "../assets/%d.png\0", i);

		textures[i - 1] = load_texture(path);

		free(path);
	}
	/*******************************************************************************/
	precomputed_renderable_target_transforms transform = calculate_rendering_grid(&board, test);//get_window_size(&window));

	loaded_shader shader = load_shader("../shaders/textured_quad.shader");

	mat4 proj;
	glm_ortho(0, 800, 800, 0, -1.0f, 1.0f, proj);
	/*******************************************************************************/

	while (!should_close(window.active_window))
	{
		poll();
		glClear(GL_COLOR_BUFFER_BIT);

		//render_board(&board);
		for (size_t i = 0; i <= board.board_size; i++)
		{

			struct game_board_segment* seg = &board.board_segments[i];
			/* Figure out a better way to handle this.*/
			vec3 pos;
			mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;

			float move[3] = { (((seg->col + 1) - 1) * transform.width) + EDGEGUARD * (seg->col + 1), (((seg->row + 1) - 1) * transform.height) + EDGEGUARD * (seg->row + 1), 0.0f };

			glm_vec3(&move, &pos);
			glm_translate(model_matrix, pos);

			if (seg->is_spacer)
				continue;
			vec2 position = { transform.width, transform.height };
			vec3 color = { 1.0, 1.0, 1.0 };

			if (seg->currently_selected)
			{
				vec3 newcolor = { .5, .5, .5 };
				glm_vec3_copy(newcolor, color);
			}

			render_textured_quad(&shader, &textures[seg->identity - 1], &model_matrix, &proj, position, color);
		}

		clear(window.active_window);

		if (has_won(&board))
		{
			printf("YOU'VE WON!!");
			system("pause");
			exit();
		}

		slide_select_two_points(&board);
		//click_two_points(&board);
	}
}