#ifndef CG2APPLICATION_H
#define CG2APPLICATION_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <cstring>
#include <sstream>
#include "gl_utils.h"
#include "geometry.h"
#include "util.h"


/* CG2App: We encapsulate all of our application state in this class.
 * We use a single instance of this object (in main). glfw will call the
 * ..._event methods when corresponding events occure.
 */
class CG2App
{
protected:
	/* GFLW backend */
	GLFWwindow *m_glfw_win;
	bool init_glfw_backend();
	void destroy_glfw_backend();


public:
	/*  Input States */
	glm::vec2 mouse_cursor;
	glm::vec2 mouse_cursor_delta;
	bool key_pressed[GLFW_KEY_LAST+1];
	bool button_pressed[GLFW_MOUSE_BUTTON_LAST+1];
	double time_stamp;
	double delta_time;
	glm::ivec2 framebuffer_size;

	/* Application States */

	CG2Geometry geo_suzanne;
	GLuint prg;

	struct LightData{
		glm::vec4 position_ws;
		glm::vec4 color;
		glm::vec4 speed; // not used by the rendering, but by the simulation
		LightData(){
			position_ws = glm::vec4(0);
			color = glm::vec4(glm::normalize(random_vec3()),1.0f);
			speed = glm::vec4(random_vec3(glm::vec3(-1.0f),glm::vec3(1.0f)),0.0f);
		}
	} lights[8];
	GLuint ubo_lights;

	void simulate_lights();
	CG2App()
	{
		init_glfw_backend();
		memset(key_pressed,0,sizeof(bool)*(GLFW_KEY_LAST+1));
		memset(button_pressed,0,sizeof(bool)*(GLFW_MOUSE_BUTTON_LAST+1));
		init_gl_state();
	}
	~CG2App()
	{
		clean_up_gl_state();
		destroy_glfw_backend();
	}
	/**
	 * @brief init_gl_state will be called once when the renderloop is
	 * entered.
	 */
	void init_gl_state();

	/**
	 * @brief clean_up_gl_state will be called after the renderloop is left
	 */
	void clean_up_gl_state();

	/**
	 * @brief resize_event Will be called every time the framebuffer is
	 * resized!
	 * @param w new width in pixels
	 * @param h new height in pixels
	 */
	void resize_event(int w, int h);

	/**
	 * @brief keyboard_event will be called every time a key on the key
	 * board is pressed or released.
	 * @param key The key (GLFW_KEY_*)
	 * @param action The action that was performed.
	 *	One of GLFW_RELEASE,GLFW_PRESS, GLFW_REPEAT
	 * @param mods The modifyer keys, that were pressed.
	 *	A bitmask composed of these bits:
	 *      GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER
	 */

	void keyboard_event(int key, int action, int mods);
	/**
	 * @brief mouse_button_event will be called every time a mouse button
	 * was pressed or released
	 * @param button The button (GLFW_MOUSE_BUTTON_*)
	 * @param action The action that was performed.
	 *	One of GLFW_RELEASE,GLFW_PRESS, GLFW_REPEAT
	 * @param mods The modifyer keys, that were pressed.
	 *	A bitmask composed of these bits:
	 *      GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER
	 */

	void mouse_button_event(int button, int action, int mods);

	/**
	 * @brief mouse_wheel_event will be called, whenever the mouse wheele is
	 * turned/shifted.
	 * @param x The offset in the x direction
	 * @param y The offset in the y direction
	 */
	void mouse_wheel_event(float x, float y);

	/**
	 * @brief render_one_frame called once per frame
	 */
	void render_one_frame();

	/**
	 * @brief stop_rendering can be called to exit the rendering loop
	 */
	void stop_rendering();

	/**
	 * @brief enter_render_loop call this to enter the renderloop
	 */
	void enter_render_loop();

	void update_title();


protected:
	// Data needed for the time measurements
	CG2GLWatch<>* m_watch[2];
	double m_gpu_time;
	double m_cpu_time;
	unsigned int m_num_timestamps;
	double m_time_passed;
};


#endif // CG2APPLICATION_H


