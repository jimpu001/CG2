#include "cg2application.h"
#include "gl_utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <algorithm>
#define UNUSED_PARAM(x) do { (void)(x); } while (0)

/**
 * @brief resize_event Will be called every time the framebuffer is
 * resized!
 * @param w new width in pixels
 * @param h new height in pixels
 */
void CG2App::resize_event(int w, int h)
{
	glViewport(0,0,w,h);
}

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

void CG2App::keyboard_event(int key, int action, int mods)
{
	UNUSED_PARAM(action); UNUSED_PARAM(mods);

	if(key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
		stop_rendering();
	if(key == GLFW_KEY_F1){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if(key == GLFW_KEY_F1 && mods & GLFW_MOD_SHIFT){

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
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

void CG2App::mouse_button_event(int button, int action, int mods)
{
	UNUSED_PARAM(button);UNUSED_PARAM(action);UNUSED_PARAM(mods);
}

/**
 * @brief mouse_wheel_event will be called, whenever the mouse wheele is
 * turned/shifted.
 * @param x The offset in the x direction
 * @param y The offset in the y direction
 */
void CG2App::mouse_wheel_event(float x, float y)
{
	UNUSED_PARAM(x);UNUSED_PARAM(y);
}


void CG2App::simulate_lights()
{
	const glm::vec3 min(-2,-0.5,-5);
	const glm::vec3 max( 2, 1, 2);
	for(int i= 0; i< 8;i++){
		LightData& l = lights[i];
		l.position_ws+=l.speed*3.0f*static_cast<float>(delta_time);

		for (int j = 0; j<3;j++) {
			if(l.position_ws[j] > max[j] ||l.position_ws[j] < min[j]){
				l.speed[j]*=-1;
				l.speed[j] =std::min(std::max(min[j],l.speed[j]),max[j]);
			}
		}
	}
}



/**
 * @brief init_gl_state will be called once when the renderloop is
 * entered.
 */
void CG2App::init_gl_state()
{
	glEnable(GL_DEPTH_TEST);
	geo_suzanne.load("data/models/suzanne.cg2vd");
	prg = cg2_load_compile_link_program("data/shaders/example.vs.glsl",
		"data/shaders/example.fs.glsl");
	glUseProgram(prg);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// (c) Create a UBO for the lights in ubo_lights
	glGenBuffers(1, &ubo_lights);
	// (d) Bind the buffer to subtarget 0 of GL_UNIFORM_BUFFER
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_lights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_lights);

}

void CG2App::clean_up_gl_state()
{
	geo_suzanne.destroyGLObjects();
	glDeleteBuffers(1,&ubo_lights);
	glDeleteProgram(prg);
}

/**
 * @brief render_one_frame called once per frame
 */
void CG2App::render_one_frame()
{
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	simulate_lights();

	// (e) Upload the freshly simulated lights
	glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(lights),lights);

	// (b)  Set the light uniforms for one of the lights[8]
	//glUniform4f(5, lights[0].position_ws.x, lights[0].position_ws.y, lights[0].position_ws.z, lights[0].position_ws.w);
	//glUniform4f(6, lights[0].color.x, lights[0].color.y, lights[0].color.z, lights[0].color.w);

	glm::mat4 P = glm::perspective(glm::radians(60.0f),static_cast<float>(framebuffer_size.x)/framebuffer_size.y,0.1f,10.0f);

	glm::vec3 camera_position(0.0f,2.0f,4.0f);
	glm::mat4 V = glm::lookAt(camera_position,glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));


	// Set the matrix uniform
	glUniformMatrix4fv(1,1,GL_FALSE,&(V[0][0]));
	glUniformMatrix4fv(2,1,GL_FALSE,&(P[0][0]));
	glUniform3fv(4,1,&(camera_position.x));
	glm::mat4 M = glm::mat4(1.0f);

	// Set the matrix uniform
	glUniformMatrix4fv(0,1,GL_FALSE,&(M[0][0]));

	// Render the geometry
	geo_suzanne.render();

}

///-----------------------------------------------------------------------------

int main (int argc, char **argv)
{
	srand(1337133742);
	UNUSED_PARAM(argc);
	UNUSED_PARAM(argv);
	CG2App app;
	app.enter_render_loop();
}

