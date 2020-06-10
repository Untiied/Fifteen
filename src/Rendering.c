#include "Rendering.h"
#include "Board.h"
#include "Window.h"
#include "SOIL.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>

precomputed_renderable_target_transforms calculate_rendering_grid(game_board* board, struct window_size size)
{
	precomputed_renderable_target_transforms info;

	/* This is the amount of space we have to work with after a left-size pixel offset of "EDGEGUARD." */
	int space_after_offset_x = size.width - (board->col_max * EDGEGUARD);

	/* This is how big our rectangle can physically be to render the texture target. */
	info.width = space_after_offset_x / board->col_max;


	// Now for the y value;

	int space_after_offset_y = size.height - (board->row_max * EDGEGUARD);

	info.height = space_after_offset_y / board->row_max;

	return info;
}

loaded_texture load_texture(const char* filepath)
{
	loaded_texture info = { 0, 0, 0 };
	char* data = SOIL_load_image(filepath, &info.width, &info.height, NULL, SOIL_LOAD_RGB);
	printf("SOIL error: %s\n", SOIL_last_result());

	if (data != NULL)
	{
		printf("Loaded image sucessfully! Image: %s\n", filepath);
		
		glGenTextures(1, &info.texture_id);
		glBindTexture(GL_TEXTURE_2D, info.texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		return info;
	}

	/* We can do this because now the image data exists on the graphics card! */
	SOIL_free_image_data(data);
	printf("Couldn't load the image!  Image: %s", filepath);
	return info;
}

/*
	The function will handle the line allocation for you. 
	@TODO : MOVE THIS TO A SEPERATE FILE.
*/
size_t getline(char** pline_buf, size_t* pn, FILE* fin)
{
	const size_t INITALLOC = 16;
	const size_t ALLOCSTEP = 16;
	size_t num_read = 0;

	if (*pline_buf == NULL)
	{
		*pline_buf = malloc(INITALLOC);
		if (pline_buf != NULL)
		{
			*pn = INITALLOC;
		}
		else
		{
			printf("[FATAL] COULDN'T ALLOCATE MEMORY FOR A LINE BUFFER!!");
			return -1;
		}
	}

	/* Go character by character to find the end line char. */
	int c;
	while (EOF != (c = getc(fin)))
	{
		num_read++;

		if (num_read >= * pn)
		{
			size_t resize = *pn + ALLOCSTEP;
			char* tmp_buff = realloc(*pline_buf, resize + 1);
			if (tmp_buff != NULL)
			{
				*pline_buf = tmp_buff;
				*pn = resize;
			}
			else
			{
				printf("[FATAL] COULDN'T REALLOCATE MEMORY FOR A LINE BUFFER!!");
				return -1;
			}

			if (SIZE_MAX < *pn)
			{
				printf("[FATAL] Reached size max!!");
				return -1;
			}


		}

		/* Actually adds the chararacter the line string. */
		(*pline_buf)[num_read - 1] = (char)c;

		if (c == '\n')
		{
			break;
		}

		if (c == EOF)
		{
			return -1;
		}
	}

	(*pline_buf)[num_read] = '\0';

	return (size_t) num_read;
}

int first = 0;
/* Quads and Tex Coords. */
/*float vertices[] = {
  -1.0f,  1.0f, 0.0f, 1.0f,
   1.0f,  1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,

  -1.0f, -1.0f, 0.0f, 0.0f,
   1.0f,  1.0f, 1.0f, 1.0f,
   1.0f, -1.0f, 1.0f, 0.0f
};*/

float vertices[] = {
   0.0f, 1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 1.0f, 1.0f,
   0.0f, 0.0f, 0.0f, 0.0f,

   0.0f, 0.0f, 0.0f, 0.0f,
   1.0f, 1.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 1.0f, 0.0f
};
void render_textured_quad(loaded_shader* shader, loaded_texture* texture, mat4* model, mat4* proj, vec2 trans, vec3 color)
{
	/*
		Remove this.
		Just have an init for the rendering segment.
	*/
	if (!first)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		first = 1;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->texture_id);

	/* 
		We would use a universal buffer so we could reduce the amount of information being sent to the GPU per render call,
		Right now this is really expensive, but because we are rendering so little things it should be okay.
		
		This gets called (n) time per frame - a further optimization would be to move alll the textures to the same UBO,
		then only updating the array on the gpu brining the (n) complexity down to zero.
		
		For now we should be okay though...
	*/
	int model_loc = glGetUniformLocation(shader->shader_id, "model");
	int projection_loc = glGetUniformLocation(shader->shader_id, "projection");
	int size_loc = glGetUniformLocation(shader->shader_id, "size");
	int color_loc = glGetUniformLocation(shader->shader_id, "color");

	glUseProgram(shader->shader_id);
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)proj);

	glUniform2fv(size_loc, 1, trans);
	glUniform3fv(color_loc, 1, color);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

/* Adapted using https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php */
loaded_shader load_shader(const char* shader_file)
{
	FILE* file = NULL;
	loaded_shader shader;
	char* vertex_string;
	char* fragment_string;
	char* current_line = NULL;
	
	file = fopen(shader_file, "r");

	if (file == NULL)
	{
		printf("Issues opening the shader file: %s", shader_file);
	}

	fseek(file, 0, SEEK_END);
	int fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	/* Allocate the memory for the string array. This could be optimized by having a seek to and after 
	the --fragment inorder to have specified memory requirements for each part of the shader. */
	vertex_string = malloc(fsize);
	vertex_string[0] = '\0';
	fragment_string = malloc(fsize);
	fragment_string[0] = '\0';

	char status;
	size_t length = 0;
	int is_fragment = 0;

	while ((status = getline(&current_line, &length, file)) != 0) {
		/* Run a check to make sure the line isn't --fragment.
			If it is --fragment switch the mode to a fragment mode. */

		if (strcmp(current_line, "--fragment\n") == 0)
		{
			is_fragment = 1;
			continue;
		}

		if (!is_fragment)
		{
			strcat(vertex_string, current_line);
		}
		else
		{
			strcat(fragment_string, current_line);
		}
	}

	uint vertex_shader_program = glCreateShader(GL_VERTEX_SHADER);
	uint fragment_shader_program = glCreateShader(GL_FRAGMENT_SHADER);

	/* This is actaully inacurate because the amount of data in one of the strings wont be the whole allocated buffer. */
	glShaderSource(vertex_shader_program, 1, &vertex_string, NULL);
	glShaderSource(fragment_shader_program, 1, &fragment_string, NULL);

	/* Check to see if the data is compiled. */
	int compiled_status = 0;
	glCompileShader(vertex_shader_program);
	glGetShaderiv(vertex_shader_program, GL_COMPILE_STATUS, &compiled_status);
	if (!compiled_status)
	{
		int log_buffer = 0;
		int length = 0;

		glGetShaderiv(vertex_shader_program, GL_INFO_LOG_LENGTH, &log_buffer);
		if (log_buffer > 1)
		{
			char* log = (char*)malloc(log_buffer);
			glGetShaderInfoLog(vertex_shader_program, log_buffer, &length, log);
			printf("Vertex Issue: %s\n", log);

			free(log);
		}
	}
	else
	{
		printf("Vertex Shader Compiled Successfully.\n");
	}

	glCompileShader(fragment_shader_program);
	glGetShaderiv(fragment_shader_program, GL_COMPILE_STATUS, &compiled_status);
	if (!compiled_status)
	{
		int log_buffer = 0;
		int length = 0;

		glGetShaderiv(fragment_shader_program, GL_INFO_LOG_LENGTH, &log_buffer);
		if (log_buffer > 1)
		{
			char* log = (char*)malloc(log_buffer);
			glGetShaderInfoLog(fragment_shader_program, log_buffer, &length, log);
			printf("Fragment Issue: %s\n", log);

			free(log);
		}
	}
	else
	{
		printf("Fragment Shader Compiled Successfully.\n");
	}

	shader.shader_id = glCreateProgram();
	glAttachShader(shader.shader_id, vertex_shader_program);
	glAttachShader(shader.shader_id, fragment_shader_program);

	/* This isn't really needed here tbh. */
	glLinkProgram(shader.shader_id);

	/* Cleanup the strings. */
	free(vertex_string);
	free(fragment_string);

	fclose(file);

	return shader;
}
