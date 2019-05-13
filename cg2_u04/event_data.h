#ifndef EVENT_DATA_H
#define EVENT_DATA_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <cstring>

struct CG2EventData
{


	/*  Input States */
	glm::vec2 mouse_cursor;
	glm::vec2 mouse_cursor_delta;
	glm::vec2 mouse_scroll_delta;

	char key_pressed[GLFW_KEY_LAST+1];
	char key_was_released[GLFW_KEY_LAST+1];
	char button_pressed[GLFW_MOUSE_BUTTON_LAST+1];
	char button_was_released[GLFW_MOUSE_BUTTON_LAST+1];

	/* Time */
	double time_stamp;
	double delta_time;

	/* Window State */
	glm::ivec2 framebuffer_size;

	/* Simulation state*/
	bool paused;
	bool cursor_catched;

	CG2EventData()
	{
		memset(key_pressed,0,sizeof(bool)*(GLFW_KEY_LAST+1));
		reset_released();
		memset(button_pressed,0,sizeof(bool)*(GLFW_MOUSE_BUTTON_LAST+1));

		paused=false;
		cursor_catched = false;
	}

	void reset_released()
	{
		memset(key_was_released,0,sizeof(bool)*(GLFW_KEY_LAST+1));
		memset(button_was_released,0,sizeof(bool)*(GLFW_MOUSE_BUTTON_LAST+1));
		mouse_cursor_delta = glm::vec2(0.0f);
		mouse_scroll_delta = glm::vec2(0.0f);
	}
};
#endif // EVENT_DATA_H
