#include "cg2application.h"
#include "gl_utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <algorithm>
#include <image.h>
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
	scene.camera.setResolution(glm::ivec2(w,h));
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
	if(key >= GLFW_KEY_1 && key <= GLFW_KEY_8 && action == GLFW_RELEASE){
		scene.global_lights.light(key-GLFW_KEY_1).toggleEnabled();
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
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		catchCursor(true);
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		catchCursor(false);

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


/**
 * @brief CG2App::simulate_lights will update all the lights positions, and
 * directions if they collide with the invisible walls. This
 * will automatically flag all lights dirty!
 */
void CG2App::simulate_lights()
{

}

/**
 * @brief CG2App::init_lights inits all light sources with random colors and
 * directions.
 */
void CG2App::init_lights()
{
	scene.global_lights.light(0).setEnabled();
	scene.global_lights.light(0).setColor(glm::vec3(1.0f,0.9f,0.8f));
	scene.global_lights.light(0).setDirection(glm::vec3(1.0f,1.0f,0.0f));
	scene.global_lights.light(0).setAmbientFactor(0.2f);
}



/**
 * @brief init_gl_state will be called once when the renderloop is
 * entered.
 */
void CG2App::init_gl_state()
{

	// Load the Shader
	prg = cg2_load_compile_link_program("data/shaders/example.vs.glsl",
	                                    "data/shaders/example.fs.glsl");
	glUseProgram(prg);

	scene.createGLObjects();
	init_lights();

	CG2Image img, img1, img2;

	// 1 (a):
	// Load the image data/textures/grass.jpg using img.
	img.load("data/textures/grass.jpg ");
	// 1 (b):
	// Generate and bind the texture tex_grass.
	glGenTextures(1, &tex_grass);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	// 1 (c):
	// Initialize tex_grass using the data from img.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixels());
	// 1 (d):
	// Set the texture parameters.
	//你可以在纹理被缩小(MIN)的时候使用邻近过滤，被放大(MAG)时使用线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 2 (a):
	// load the other textures (data/textures/pebble.jpg and data/textures/distribution.png) in
	// tex_pebble and tex_distribution accordingly.
	img1.load("data/textures/pebble.jpg ");
	img2.load("data/textures/distribution.png ");
	glGenTextures(1, &tex_pebble);
	glGenTextures(1, &tex_distribution);
	// 2 (b):
	// Bind the three textures to the target GL_TEXTURE_2D of the texture units one, two and three.
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_pebble);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img1.getWidth(), img1.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img1.getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//必须分开绑定纹理和参数设置
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_distribution);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img2.getWidth(), img2.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img2.getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// load geometry
	scene.getGeometry("geo_floor")->load("data/models/floor.cg2vd");

	// create the CG2Objects, set their geometry object and position them
	// in the scene.
	scene.placeObject("floor","geo_floor");
	scene.camera.lookAt(glm::vec3(12.0f,4.0f,12.0f),glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));

	// set some gl states
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8f,0.9f,0.9f,1.0f);

}

/**
 * @brief CG2App::clean_up_gl_state this is called before the GLContext is
 * destroyed. Destroy every GLObject from here!
 */
void CG2App::clean_up_gl_state()
{
	scene.destroyGLObjects();
	glDeleteProgram(prg);
}


/**
 * @brief render_one_frame called once per frame
 */
void CG2App::render_one_frame()
{

	scene.camera.tick(event_data);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	simulate_lights();


	scene.render();

}

///-----------------------------------------------------------------------------

int main (int argc, char **argv)
{
	UNUSED_PARAM(argc);
	UNUSED_PARAM(argv);
	srand(1337133742);
	CG2App app;
	app.enter_render_loop();
}

