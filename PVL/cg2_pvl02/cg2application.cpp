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
	const glm::vec3 min(-2.0f,-0.5f,-2.0f);
	const glm::vec3 max( 2.0f, 1.0f, 2.0f);
	for(int i= 0; i< 8;i++){
		uboLightData& l = scene.global_lights.light(i);
		l.position+= glm::vec4(light_directions[i]*3.0f*static_cast<float>(event_data.delta_time),0.0f);

		for (int j = 0; j<3;j++) {
			if(l.position[j] > max[j] ||l.position[j] < min[j]){
				light_directions[i][j]*=-1;
				light_directions[i][j] =std::min(std::max(min[j],light_directions[i][j]),max[j]);
			}
		}
	}
}

/**
 * @brief CG2App::init_lights inits all light sources with random colors and
 * directions.
 */
void CG2App::init_lights()
{
	for(int i  =0 ; i< CG2_MAX_GLOBAL_LIGHTS; i++){
		scene.global_lights.light(i).setColor(random_clr());
		scene.global_lights.light(i).setPosition(glm::vec3(0,1,0));
		scene.global_lights.light(i).setAmbientFactor(0.02f);
		scene.global_lights.light(i).setEnabled();
		light_directions[i] = glm::normalize(random_vec3(glm::vec3(-1),glm::vec3(1)));
	}
}



/**
 * @brief init_gl_state will be called once when the renderloop is
 * entered.
 */
void CG2App::init_gl_state()
{
	// Initialization
	scene.createGLObjects();
	init_lights();

	// Load the Shader
	prg = cg2_load_compile_link_program("data/shaders/example.vs.glsl",
	                                    "data/shaders/example.fs.glsl");
	glUseProgram(prg);


	// Load geometry
	scene.getGeometry("geo_suzanne")->load("data/models/suzanne.cg2vd");
	scene.getGeometry("geo_floor")->load("data/models/floor.cg2vd");

	// Create the CG2Objects, set their geometry object and position them
	// in the scene.
	scene.placeObject("floor","geo_floor");
	scene.placeObject("suzanne0","geo_suzanne");
	scene.placeObject("suzanne1", "geo_suzanne", glm::vec3(3.0, 0.0, 0.0));
	scene.placeObject("suzanne2", "geo_suzanne", glm::vec3(-3.0, 0.0, 0.0));


	// Set the camera's initial position and orientation.
	scene.camera.lookAt(glm::vec3(0.0f,2.0f,6.0f),glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));
	glEnable(GL_DEPTH_TEST);


	// Dummy Matrices for camera parameters. You can remove this after finishing PVL2 Task 1.
	//glm::mat4 V = glm::lookAt(glm::vec3(0.0f,2.0f,6.0f),glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));
	//glm::mat4 iV = glm::inverse(V);
	//glm::mat4 P = glm::perspective(glm::radians(45.0f),1.0f,0.1f,60.0f);
	//glUniformMatrix4fv(0,1,GL_FALSE,&(V[0][0]));
	//glUniformMatrix4fv(1,1,GL_FALSE,&(P[0][0]));
	//glUniformMatrix4fv(2,1,GL_FALSE,&(iV[0][0]));

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Simulate the camera.
	scene.camera.tick(event_data);

	// Simulate the bouncing lights.
	simulate_lights();

	// Spin the monkey's head.
	scene.getObject("suzanne0")->rotateModelMatrix(static_cast<float>(event_data.delta_time),
	                                               0.0f,1.0f,0.0f);
	scene.getObject("suzanne1")->rotateModelMatrix(static_cast<float>(event_data.delta_time),
		0.0f, 1.0f, 0.0f);
	
	// Render the whole Scene.
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

