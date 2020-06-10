#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct
{
	int window_height;
	int window_width;

	const char* window_name;

	GLFWwindow* active_window;
} window_payload;

struct window_size
{
	int width;
	int height;
};

typedef struct
{
	double x;
	double y;
} mouse_position;

enum mouse_button
{
	/* Per the GLFW docs. */
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2,
};

/* 0 will be false aka not pushed, 1 will be true aka pushed. */
int mouse_buttons[3];

int mouse_buttons_intermediate[3];

mouse_position current_mouse_position;

void create_window(window_payload* winfo);

void poll();

/* Controls the glfw viewport update instruction, passes it to opengl. */
void update_viewport_glfw(GLFWwindow* window, int width, int height);

void update_mouse_button_state(GLFWwindow* window, int button, int action, int mods);

void update_mouse_position(GLFWwindow* window, double xpos, double ypos);
/* This will return the state of the given button. */
int get_mouse_state(int button);

int get_mouse_down(int button);

int get_mouse_up(int button);

inline set_mouse_state(int button, int state)
{
	mouse_buttons[button] = state;
}

mouse_position* get_mouse_position(GLFWwindow* window);

void clear(GLFWwindow* window);

int should_close(GLFWwindow* window);

struct window_size get_window_size(GLFWwindow* window);

#endif // !WINDOW_H



