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

	if(key == GLFW_KEY_R && action == GLFW_RELEASE)
		scene.reloadAllShaders();
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
 * @brief CG2App::init_lights inits all light sources.
 */
void CG2App::init_lights()
{
	scene.global_lights.light(0).setEnabled();
	scene.global_lights.light(0).setColor(glm::vec3(0.8f,0.7f,0.5f));
	scene.global_lights.light(0).setDirection(glm::vec3(1.0f,1.0f,0.0f));
	scene.global_lights.light(0).setAmbientFactor(0.2f);

	scene.global_lights.light(1).setEnabled();
	scene.global_lights.light(1).setColor(glm::vec3(0.5f,0.5f,0.5f));
	scene.global_lights.light(1).setDirection(glm::vec3(-1.0f,-1.0f,0.0f));
	scene.global_lights.light(1).setAmbientFactor(0.2f);
}

void CG2App::init_shader()
{

	scene.buildShaderProgramm("cm",{
	                                  "data/shaders/vtx_tf_static_full.vert",
	                                  "data/shaders/frg_albedo_tex_only.frag",
	                                  "data/shaders/frg_material_props_mat.frag",
	                                  "data/shaders/frg_lighting_phong.frag",
	                                  "data/shaders/frg_normal_attrib.frag"},true);

	scene.buildShaderProgramm("sky",{
	                                  "data/shaders/vtx_tf_skybox.vert",
	                                  "data/shaders/frg_albedo_skybox.frag",
	                                  "data/shaders/frg_material_props_mat.frag",
	                                  "data/shaders/frg_lighting_none.frag",
	                                  "data/shaders/frg_normal_attrib.frag"},true);
	// 2 (b):
	// scene.buildShaderProgramm("cmat" ...
}



void CG2App::init_sponza_scene()
{
	char strbuff[4]={0,0,0,0};

	CG2Image img;
	for(int i = 0; i< 25;i++){
		sprintf(strbuff,"%03d",i);
		std::string num_str = strbuff;
		scene.getGeometry("geo_sponza_"+num_str)->load("data/models/sponza/sponza."+num_str+".cg2vd");
		img.load("data/textures/sponza/"+num_str+"a.tga");
		scene.getTexture("tex_sponza_"+num_str)->createFrom(img,16.0f);
		CG2Material* mat = scene.getMaterial("mat_sponza_"+num_str);
		mat->albedo_map = scene.getTexture("tex_sponza_"+num_str);
		mat->shader = scene.getProgram("cm");
		scene.placeObject("sponza_"+num_str,"geo_sponza_"+num_str,"mat_sponza_"+num_str);
	}



	// 2 (c):
	// ...



	CG2Material* sky_mat = scene.getMaterial("mat_sky");

	sky_mat->albedo_map = scene.getTexture("tex_sky");
	sky_mat->shader = scene.getProgram("sky");


	img.load("data/textures/tropicalSunnyDay/xp.jpg");
	sky_mat->albedo_map->setCubeMapSideFrom(img,GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	img.load("data/textures/tropicalSunnyDay/xn.jpg");
	sky_mat->albedo_map->setCubeMapSideFrom(img,GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	img.load("data/textures/tropicalSunnyDay/yp.jpg");
	sky_mat->albedo_map->setCubeMapSideFrom(img,GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	img.load("data/textures/tropicalSunnyDay/yn.jpg");
	sky_mat->albedo_map->setCubeMapSideFrom(img,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	img.load("data/textures/tropicalSunnyDay/zp.jpg");
	sky_mat->albedo_map->setCubeMapSideFrom(img,GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	img.load("data/textures/tropicalSunnyDay/zn.jpg");
	sky_mat->albedo_map->setCubeMapSideFrom(img,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	sky_mat->albedo_map->bind(CG2_TEXTURE_UNIT_SKY_MAP);

	CG2Geometry* sky_geo = scene.getGeometry("geo_sky");
	sky_geo->createEmpty(GL_TRIANGLE_STRIP,4);

	scene.placeObject("sky","geo_sky","mat_sky");


}



/**
 * @brief init_gl_state will be called once when the renderloop is
 * entered.
 */
void CG2App::init_gl_state()
{


	scene.createGLObjects();

	scene.shader_comp.start_compilation_cycle();
	init_shader();


	init_lights();
	init_sponza_scene();
	scene.shader_comp.finish_compilation_cycle();
	scene.camera.lookAt(glm::vec3(10.0f,7.0f,-1.50f),glm::vec3(0.0f,3.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));

	// set some gl states
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8f,0.9f,0.9f,1.0f);
	glDepthFunc(GL_LEQUAL);

}

/**
 * @brief CG2App::clean_up_gl_state this is called before the GLContext is
 * destroyed. Destroy every GLObject from here!
 */
void CG2App::clean_up_gl_state()
{
	scene.destroyGLObjects();
}


/**
 * @brief render_one_frame called once per frame
 */
void CG2App::render_one_frame()
{

	scene.camera.tick(event_data);

	// 3 (c):
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render(BVTX_FULL,BFRG_WRITE_COLORBUFF);


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

