#include "Window.h"

void create_window(window_payload* winfo)
{
	int result = glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (result != GLFW_TRUE)
	{
		printf("Failed to start GLFW!");
		return;
	}

	winfo->active_window = glfwCreateWindow(winfo->window_width, winfo->window_height, winfo->window_name, NULL, NULL);
	glfwMakeContextCurrent(winfo->active_window);

	/* This will allow our dragging to happen. */
	/* @TODO: There are better ways to handle this. */
	//glfwSetInputMode(winfo->active_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

	/*************************************/
	glfwSetWindowSizeCallback(winfo->active_window, update_viewport_glfw);
	glfwSetMouseButtonCallback(winfo->active_window, update_mouse_button_state);
	glfwSetCursorPosCallback(winfo->active_window, update_mouse_position);
	/*************************************/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void update_viewport_glfw(GLFWwindow* window, int width, int height)
{
	/* We might need to adjust the x, y value for when we add zooming. */
	glViewport(0, 0, width, height);
}

void update_mouse_button_state(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
		case GLFW_PRESS:
			mouse_buttons[button] = 1;
			return;

		case GLFW_RELEASE:
			mouse_buttons[button] = 0;
			//mouse_buttons_intermediate[button] = 0;
			return;

		case GLFW_REPEAT:
			return;

		default:
			break;
	}
}

void update_mouse_position(GLFWwindow* window, double xpos, double ypos)
{
	current_mouse_position.x = xpos;
	current_mouse_position.y = ypos;
}

int get_mouse_state(int button)
{
	return mouse_buttons[button];
}

int get_mouse_down(int button)
{
	if (mouse_buttons[button] == 1 && mouse_buttons_intermediate[button] == 0)
	{
		mouse_buttons_intermediate[button] = 1;
		return 1;
	}

	return 0;
}

int get_mouse_up(int button)
{
	if (mouse_buttons[button] == 0 && mouse_buttons_intermediate[button] == 1)
	{
		mouse_buttons_intermediate[button] = 0;
		return 1;
	}

	return 0;
}

mouse_position* get_mouse_position(GLFWwindow* window)
{
	return &current_mouse_position;
}

void poll()
{
	glfwPollEvents();
}

void clear(GLFWwindow* window)
{
	glfwSwapBuffers(window);
}

int should_close(GLFWwindow* window)
{
	return glfwWindowShouldClose(window);
}

struct window_size get_window_size(GLFWwindow* window)
{
	struct window_size info = { 0 };
	glfwGetFramebufferSize(window, &info.width, &info.height);
	
	return info;
}