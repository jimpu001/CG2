#ifndef U09SHADOWMAPPING_H
#define U09SHADOWMAPPING_H
#include "gl_utils.h"
#include "texture.h"
#include "scene.h"
#include "camera.h"
#include "lights.h"
#include "bindings_locations.h"

#define GL_SHADOW_MAP_WIDTH 1024
#define GL_SHADOW_MAP_HEIGHT 1024

class CG2ShadowMapping
{
	GLsizei width;
	GLsizei height;
	GLuint fbo;
	CG2Texture tex_shadow_map;
	CG2Scene* scene;
	CG2Camera light_cam;
	GLint tex_filter_param;

	void set_camera(int glight_id);
public:
	CG2ShadowMapping();

	void resizeShadowMap(GLsizei w, GLsizei h);

	void createGLObjects(CG2Scene& scene, GLsizei w, GLsizei h);

	void changeTextureFilter();

	void renderShadowMap();
	void destroyGLObjects();


};

#endif // U09SHADOWMAPPING_H
