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
	camera.setResolution(glm::ivec2(w,h));
	post_processing.updateResolution(w,h);
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

	if(key == GLFW_KEY_ESCAPE)
		stop_rendering();
	if(key == GLFW_KEY_F1){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if(key == GLFW_KEY_F1 && mods & GLFW_MOD_SHIFT){

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(key == GLFW_KEY_Q){
		scene.global_lights.light(1).setPosition(camera.pos_es_ws(glm::vec3(0,0,-0.1f)));

	}
	if(key == GLFW_KEY_E){
		scene.global_lights.light(2).setPosition(camera.pos_es_ws(glm::vec3(0,0,-0.1f)));
	}

	if(key == GLFW_KEY_L){
		scene.global_lights.light(0).setPosition(camera.pos_es_ws(glm::vec3(0.25,-0.25,0)));
		scene.global_lights.light(0).setSpotDirection(camera.dir_es_ws(glm::vec3(0,0,-1)));		
	}

	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_KP_4:
				post_processing.ppData().brightness_contrast_gamma_saturation.x += 0.1f; break;

			case GLFW_KEY_KP_1:
				post_processing.ppData().brightness_contrast_gamma_saturation.x -= 0.1f; break;

			case GLFW_KEY_KP_5:
				post_processing.ppData().brightness_contrast_gamma_saturation.y += 0.1f; break;

			case GLFW_KEY_KP_2:
				post_processing.ppData().brightness_contrast_gamma_saturation.y -= 0.1f; break;

			case GLFW_KEY_KP_6:
				post_processing.ppData().brightness_contrast_gamma_saturation.z += 0.1f; break;

			case GLFW_KEY_KP_3:
				post_processing.ppData().brightness_contrast_gamma_saturation.z += 0.1f; break;

			case GLFW_KEY_KP_MULTIPLY:
				post_processing.ppData().brightness_contrast_gamma_saturation.w += 0.1f; break;

			case GLFW_KEY_KP_9:
				post_processing.ppData().brightness_contrast_gamma_saturation.w -= 0.1f; break;

			case GLFW_KEY_KP_DIVIDE:
				post_processing.ppData().boxSize_e_e_e.x += 1.0f; break;

			case GLFW_KEY_KP_8:
				post_processing.ppData().boxSize_e_e_e.x -= 1.0f;
				post_processing.ppData().boxSize_e_e_e.x = std::max(1.0f, post_processing.ppData().boxSize_e_e_e.x);
				break;


			//case GLFW_KEY_UP:
				//post_processing.ppData().focalDistance_focalSize_maxLevel_e.x += 0.25f; break;
			
			//case GLFW_KEY_DOWN:
				//post_processing.ppData().focalDistance_focalSize_maxLevel_e.x -= 0.25f; break;

			//case GLFW_KEY_LEFT:
				//post_processing.ppData().focalDistance_focalSize_maxLevel_e.y -= 0.25f; break;

			//case GLFW_KEY_RIGHT:
				//post_processing.ppData().focalDistance_focalSize_maxLevel_e.y += 0.25f; break;

			case GLFW_KEY_PAGE_UP:
				post_processing.ppData().focalDistance_focalSize_maxLevel_e.z += 0.25f; break;

			case GLFW_KEY_PAGE_DOWN:
				post_processing.ppData().focalDistance_focalSize_maxLevel_e.z -= 0.25f; break;


			case GLFW_KEY_R:
				scene.reloadAllShaders();

			case GLFW_KEY_6:
				shadow_mapping.resizeShadowMap(128, 128); break;

			case GLFW_KEY_7:
				shadow_mapping.resizeShadowMap(256, 256); break;

			case GLFW_KEY_8:
				shadow_mapping.resizeShadowMap(512, 512); break;

			case GLFW_KEY_9:
				shadow_mapping.resizeShadowMap(1024, 1024); break;

			case GLFW_KEY_K:
				scene.global_lights.light(0).spotExp_radius_e_enabled.z += 2.0f;
				printf("shadow filter size: %.0f\n", scene.global_lights.light(0).spotExp_radius_e_enabled.z);
				break;

			case GLFW_KEY_J:
				scene.global_lights.light(0).spotExp_radius_e_enabled.z =
					std::max(1.0f, scene.global_lights.light(0).spotExp_radius_e_enabled.z - 2.0f);
				printf("shadow filter size: %.0f\n", scene.global_lights.light(0).spotExp_radius_e_enabled.z);
				break;

			case GLFW_KEY_H:
				shadow_mapping.changeTextureFilter();
				break;

		}
	}
	
	if(key >= GLFW_KEY_1 && key <= GLFW_KEY_4 && action == GLFW_RELEASE)
		scene.global_lights.light(key-GLFW_KEY_1).toggleEnabled();
	if (key==GLFW_KEY_F4&& action == GLFW_RELEASE) {
		if (anime == false) {
			anime = true;
		}
		else
			anime = false;
	}
}


void CG2App::light_tick(const CG2EventData& eventData) {
	
	if (anime) {
		if (scene.global_lights.light(0).position.x != 5.0f
			&&scene.global_lights.light(0).position.z != 2.0f
			&&scene.global_lights.light(0).position.x != -5.0f
			&&scene.global_lights.light(0).position.z != -2.0f)
			scene.global_lights.light(0).position.x += 0.025f;

		if (scene.global_lights.light(0).position.x == 5.0f&&scene.global_lights.light(0).position.z != 2.0f)
			scene.global_lights.light(0).position.z += 0.025f;

		if (scene.global_lights.light(0).position.z == 2.0f&&scene.global_lights.light(0).position.x == 5.0f)
			scene.global_lights.light(0).position.x -= 0.025f;
		if (scene.global_lights.light(0).position.z == 2.0f&&scene.global_lights.light(0).position.x != 5.0f)
			scene.global_lights.light(0).position.x -= 0.025f;

		if (scene.global_lights.light(0).position.x == -5.0f&&scene.global_lights.light(0).position.z == 2.0f)
			scene.global_lights.light(0).position.z -= 0.025f;
		if (scene.global_lights.light(0).position.x == -5.0f&&scene.global_lights.light(0).position.z != 2.0f)
			scene.global_lights.light(0).position.z -= 0.025f;

		if (scene.global_lights.light(0).position.z == -2.0f&&scene.global_lights.light(0).position.x == -5.0f)
			scene.global_lights.light(0).position.x += 0.025f;
		if (scene.global_lights.light(0).position.z == -2.0f&&scene.global_lights.light(0).position.x != -5.0f)
			scene.global_lights.light(0).position.x += 0.025f;

		if (scene.global_lights.light(0).position.x == 5.0f&&scene.global_lights.light(0).position.z == -2.0f)
			scene.global_lights.light(0).position.z += 0.025f;
		if (scene.global_lights.light(0).position.x == 5.0f&&scene.global_lights.light(0).position.z != -2.0f)
			scene.global_lights.light(0).position.z += 0.025f;

	}
	else {
		if (eventData.key_pressed[GLFW_KEY_UP]) {
			scene.global_lights.light(0).position.x += 0.25f;
		}
		if (eventData.key_pressed[GLFW_KEY_DOWN]) {
			scene.global_lights.light(0).position.x -= 0.25f;
		}
		if (eventData.key_pressed[GLFW_KEY_LEFT]) {
			scene.global_lights.light(0).position.z -= 0.25f;
		}
		if (eventData.key_pressed[GLFW_KEY_RIGHT]) {
			scene.global_lights.light(0).position.z += 0.25f;
		}
	}
	if (scene.global_lights.light(0).position.x > 5.0f)scene.global_lights.light(0).position.x = 5.0f;
	if (scene.global_lights.light(0).position.x < -5.0f)scene.global_lights.light(0).position.x = -5.0f;
	if (scene.global_lights.light(0).position.z > 2.0f)scene.global_lights.light(0).position.z = 2.0f;
	if (scene.global_lights.light(0).position.z < -2.0f)scene.global_lights.light(0).position.z = -2.0f;
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

	// control focalDistance with mouse wheel or cursor UP/DOWN
	if (fabsf(y) > 0.01f) {
		post_processing.ppData().focalDistance_focalSize_maxLevel_e.x += 0.25f * y;
	}

}

/**
 * @brief CG2App::init_lights inits all light sources.
 */
void CG2App::init_lights()
{
	scene.global_lights.light(0).setEnabled();
	scene.global_lights.light(0).setColor(glm::vec3(1.0f,0.9f,0.7f)*2.0f);
	scene.global_lights.light(0).setPosition(glm::vec3(0.1f, 15.0f, 0.0f));
	//scene.global_lights.light(0).setSpotDirection(glm::vec3(1.0f,-0.9f, 0.0f));
	//scene.global_lights.light(0).setSpotCosCutoff(cosf(0.5));
	//scene.global_lights.light(0).setDirection(glm::vec3(0.1f, 15.0f, 0.0f));
	scene.global_lights.light(0).setAmbientFactor(0.01f);

	//scene.global_lights.light(1).setEnabled();
	scene.global_lights.light(1).setColor(glm::vec3(1.0f,0.5f,0.0f)*5.0f);
	scene.global_lights.light(1).setPosition(glm::vec3(0.0f));
	scene.global_lights.light(1).setAmbientFactor(0.000f);
	scene.global_lights.light(1).setRadius(2);

	//scene.global_lights.light(2).setEnabled();
	scene.global_lights.light(2).setColor(glm::vec3(0.0f,0.5f,1.0f)*5.0f);
	scene.global_lights.light(2).setPosition(glm::vec3(0.0f));
	scene.global_lights.light(2).setAmbientFactor(0.000f);
	scene.global_lights.light(2).setRadius(2);


	//scene.global_lights.light(3).setEnabled();
	scene.global_lights.light(3).setColor(glm::vec3(1.0f,0.9f,0.7f)*1.0f);
	scene.global_lights.light(3).setDirection(glm::vec3(-1.0f,2.0f,0.0f));
	scene.global_lights.light(3).setAmbientFactor(0.01f);

	scene.global_lights.light(4).setEnabled();
	scene.global_lights.light(4).setColor(glm::vec3(1.0f,0.9f,0.7f)*0.15f);
	scene.global_lights.light(4).setDirection(glm::vec3(1.0f,2.0f,0.0f));
	scene.global_lights.light(4).setAmbientFactor(0.01f);

}

void CG2App::init_shader()
{

	scene.buildShaderProgramm("cmat",{
	                                  "data/shaders/vtx_tf_static_full.vert",
	                                  "data/shaders/frg_material_props_mat.frag",
	                                  "data/shaders/frg_lighting_pbs.frag",
	                                  "data/shaders/frg_albedo_mat_only.frag",
	                                  "data/shaders/frg_normal_attrib.frag",
	                                  "data/shaders/frg_shadow_map.frag"},true);


	scene.buildShaderProgramm("cm",{
	                                  "data/shaders/vtx_tf_static_full.vert",
	                                  "data/shaders/frg_material_props_mat.frag",
	                                  "data/shaders/frg_lighting_pbs.frag",
	                                  "data/shaders/frg_albedo_tex_only.frag",
	                                  "data/shaders/frg_normal_attrib.frag",
	                                  "data/shaders/frg_shadow_map.frag"},true);

	scene.buildShaderProgramm("cm_nm",{
	                                  "data/shaders/vtx_tf_static_full.vert",
	                                  "data/shaders/frg_material_props_mat.frag",
	                                  "data/shaders/frg_lighting_pbs.frag",
	                                  "data/shaders/frg_albedo_tex_only.frag",
	                                  "data/shaders/frg_normal_map.frag",
	                                  "data/shaders/frg_shadow_map.frag"},true);

	scene.buildShaderProgramm("cm_nm_mm",{
	                                  "data/shaders/vtx_tf_static_full.vert",
	                                  "data/shaders/frg_material_props_map.frag",
	                                  "data/shaders/frg_lighting_pbs.frag",
	                                  "data/shaders/frg_albedo_tex_only.frag",
	                                  "data/shaders/frg_normal_map.frag",
	                                  "data/shaders/frg_shadow_map.frag"},true);


	scene.buildShaderProgramm("sky",{
	                                  "data/shaders/vtx_tf_skybox.vert",
	                                  "data/shaders/frg_albedo_skybox.frag",
	                                  "data/shaders/frg_material_props_mat.frag",
	                                  "data/shaders/frg_lighting_none.frag",
	                                  "data/shaders/frg_normal_map.frag",
	                                  "data/shaders/frg_shadow_none.frag"},true);
}

void CG2App::init_sponza_scene()
{
	char strbuff[4]={0,0,0,0};


	glm::vec2 scale_offset[] = {
	        glm::vec2(0.5,0.5),//0
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.5,0.5),
	        glm::vec2(1.0,0.0),
	        glm::vec2(1.0,0.0),//5
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.0,1.0),
	        glm::vec2(1.0,0.0),
	        glm::vec2(1.0,0.0),
	        glm::vec2(1.0,0.0),//10
	        glm::vec2(1.0,0.0),
	        glm::vec2(1.0,0.0),
	        glm::vec2(1.0,0.0),
	        glm::vec2(0.5,0.5),
	        glm::vec2(1.0,0.0),//15
	        glm::vec2(1.0,0.0),
	        glm::vec2(1.0,0.0),
	        glm::vec2(0.0,1.0),
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.5,0.5),//20
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.5,0.5),
	        glm::vec2(0.5,0.5),
	};
	CG2Image img;
	for(int i = 0; i< 25;i++){
		sprintf(strbuff,"%03d",i);
		std::string num_str = strbuff;
		scene.getGeometry("geo_sponza_" + num_str)->load("data/models/sponza/sponza." + num_str + ".cg2vd");

		CG2Material * mat = scene.getMaterial("mat_sponza_" + num_str);

		img.load("data/textures/sponza/" + num_str + "a.tga");
		scene.getTexture("tex_sponza_" + num_str + "a")->createFrom(img, 16.0f);
		mat->albedo_map = scene.getTexture("tex_sponza_" + num_str + "a");

		bool got_normal_map = false;
		bool got_material_map = false;

		if(img.loadLinear("data/textures/sponza/" + num_str + "n.tga")){
			scene.getTexture("tex_sponza_" + num_str + "n")->createFrom(img, 16.0f);
			mat->normal_map = scene.getTexture("tex_sponza_" + num_str + "n");
			got_normal_map = true;

		}

		if(img.loadLinear("data/textures/sponza/" + num_str + "mp.tga")){
			scene.getTexture("tex_sponza_" + num_str + "mp")->createFrom(img, 16.0f);
			mat->material_properies_map = scene.getTexture("tex_sponza_" + num_str + "mp");
			got_material_map = true;

		}


		if(got_normal_map && got_material_map){
			mat->shader = scene.getProgram("cm_nm_mm");
		}
		else if (got_normal_map){
			mat->shader = scene.getProgram("cm_nm");
		}
		else {
			mat->shader = scene.getProgram("cm");
		}

		mat->setRoughnessScale(scale_offset[i].x);
		mat->setRoughnessOffset(scale_offset[i].y);

		scene.placeObject("sponza_" + num_str, "geo_sponza_" + num_str, "mat_sponza_" + num_str);
	}



	CG2Material* mat = scene.getMaterial("mat_trex");
	mat->setRoughness(0.2f);
	mat->setMetallic(1.0f);
	mat->setAlbedo(glm::vec4(1.0f,0.5f,0.0f,1.0f));
	mat->shader = scene.getProgram("cmat");

	scene.getGeometry("geo_trex")->load("data/models/trex.cg2vd");

	scene.placeObject("trex","geo_trex","mat_trex");



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

	scene.getGeometry("geo_sky")->createEmpty(GL_TRIANGLE_STRIP,4);
	scene.placeObject("sky","geo_sky","mat_sky");
}

/**
 * @brief init_gl_state will be called once when the renderloop is
 * entered.
 */
void CG2App::init_gl_state()
{
	init_lights();

	scene.createGLObjects();
	camera.createGLObjects();
	camera.lookAt(glm::vec3(10.0f, 7.0f, -1.50f), glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	post_processing.createGLObjects(scene);
	shadow_mapping.createGLObjects(scene, GL_SHADOW_MAP_WIDTH, GL_SHADOW_MAP_HEIGHT);

	init_shader();	
	init_sponza_scene();

	


	// set some gl states
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8f,0.9f,0.9f,1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_FRAMEBUFFER_SRGB);
		
}

/**
 * @brief CG2App::clean_up_gl_state this is called before the GLContext is
 * destroyed. Destroy every GLObject from here!
 */
void CG2App::clean_up_gl_state()
{
	scene.destroyGLObjects();
	camera.destroyGLObjects();
	post_processing.destroyGLObjects();
	shadow_mapping.destroyGLObjects();
}

/**
 * @brief render_one_frame called once per frame
 */
void CG2App::render_one_frame()
{
	light_tick(event_data);
	camera.tick(event_data);
	CG2_PUSH_DBG_GROUP("renderShadowMap");
	shadow_mapping.renderShadowMap();
	CG2_POP_DBG_GROUP();
	camera.use();

	post_processing.setupFramebuffer(post_processing.getInternalTexture(0));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CG2_PUSH_DBG_GROUP("Depth-Pre-Pass");
	glDepthFunc(GL_LEQUAL);
	scene.render(BVTX_FULL,BFRG_NULL);
	CG2_POP_DBG_GROUP();

	CG2_PUSH_DBG_GROUP("Main-Pass");
	glDepthFunc(GL_EQUAL);
	scene.render(BVTX_FULL,BFRG_WRITE_COLORBUFF);
	CG2_POP_DBG_GROUP();

	CG2_PUSH_DBG_GROUP("Post-Processing");
	glDisable(GL_DEPTH_TEST);

	CG2_PUSH_DBG_GROUP("gauss blur all levels");
	post_processing.doGaussBlur4DoF(post_processing.getInternalTexture(0));
	CG2_POP_DBG_GROUP();

	CG2_PUSH_DBG_GROUP("DoF");
	post_processing.doDoFEffect(post_processing.getInternalTexture(0),post_processing.getInternalDepthTexture(),post_processing.getInternalTexture(1));
	CG2_POP_DBG_GROUP();
	//post_processing.doGaussBlur(post_processing.getInternalTexture(0),post_processing.getInternalTexture(1));

	CG2_PUSH_DBG_GROUP("color correction");
	post_processing.doColorCorrection(post_processing.getInternalTexture(1),nullptr);

	glEnable(GL_DEPTH_TEST);
	CG2_POP_DBG_GROUP();
	CG2_POP_DBG_GROUP();
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

