#ifndef RENDERING_H
#define RENDERING_H
#define EDGEGUARD 2
#include <cglm/mat4.h>

/* This is how large a single renderable texture is allowed to be. */
typedef struct
{
	float width;
	float height;
	/* For a texture this will always be the top left of the "rectangle." */
} precomputed_renderable_target_transforms;

typedef struct
{
	int width;
	int height;

	unsigned int texture_id;
} loaded_texture;

typedef struct
{
	unsigned int shader_id;
} loaded_shader;

unsigned int vao;
unsigned int vbo;

/* IMPLEMENTATION OF C's GET LINE. */
size_t getline(char** pline_buf, size_t* pn, struct FILE* fin);

precomputed_renderable_target_transforms calculate_rendering_grid(struct game_board* board, struct window_size size);

void render_textured_quad(loaded_shader* shader, loaded_texture* texture, mat4* model, mat4* proj, precomputed_renderable_target_transforms* trans, vec3 color);

loaded_texture load_texture(const char* filepath);

loaded_shader load_shader(const char* shader_files);

#endif // !RENDERING_H
