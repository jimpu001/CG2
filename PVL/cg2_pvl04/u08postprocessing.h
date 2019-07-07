#ifndef U08POSTRPOCESSING_H
#define U08POSTRPOCESSING_H

#include <glad/glad.h>
#include "texture.h"
#include "geometry.h"
#include "scene.h"
#include "event_data.h"
#include "bindings_locations.h"
#include "shader_compositor.h"
struct uboPostProcessingData
{
	// color
	glm::vec4 brightness_contrast_gamma_saturation;
	// box blur
	glm::vec4 boxSize_e_e_e;
	// gaussian blur 1D
	glm::vec4 direction_level_e;
	// depth-of-field
	glm::vec4 focalDistance_focalSize_maxLevel_e;
	// ghost-related
	glm::vec4 viewport; /* x y w h */
	glm::vec4 ghost;
	glm::vec4 ghost_center_radius;

	// initialize in constructor
	uboPostProcessingData() :
	        brightness_contrast_gamma_saturation(glm::vec4(0.0f,1.0f,1.0f,1.0f)),
	        boxSize_e_e_e(glm::vec4(1.0f,0.0f,0.0f,0.0f)),
	        direction_level_e(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)),
	        focalDistance_focalSize_maxLevel_e(glm::vec4(10.0f,5.0f,0.0f,0.0f)),
	        viewport(glm::vec4(0.0f,0.0f,800.0f,600.0f)),
	        ghost(glm::vec4(0.0f,0.0f,0.0f,0.0f)),
	        ghost_center_radius(glm::vec4(0.0f,0.0f,0.0f,1.0f))
	{
	}
};

enum PostProcessingEffect { CG2_PP_NONE = 0, CG2_PP_COLOR_CORRECTION = 1, CG2_PP_BOX_BLUR = 2, CG2_PP_GAUSSIAN_BLUR = 3, CG2_PP_DOF = 4 };

class CG2Postprocessing
{
	uboPostProcessingData data;
	GLuint ubo_pp;
	bool dirty_ubo;

	PostProcessingEffect pp_effect;

	GLuint fbo;
	CG2Texture tex_depth;
	CG2Texture tex_color[2];
	CG2Texture tex_blur;

	CG2Geometry screen_filling_quad;

	CG2ShaderProgram* color_corection;
	CG2ShaderProgram* box_blur;
	CG2ShaderProgram* gauss_blur;
	CG2ShaderProgram* dof;


public:

	CG2Texture* getInternalTexture(GLuint id){return &(tex_color[id%2]);}
	CG2Texture* getInternalDepthTexture(){return &tex_depth;}
	CG2Postprocessing();

	void updateResolution(int w, int h);

	const uboPostProcessingData& ppData() const;
	uboPostProcessingData& ppData();

	PostProcessingEffect& ppEffect();

	void createGLObjects(CG2Scene& scene);
	void destroyGLObjects();
	void setupFramebuffer(CG2Texture *target_texture_name,const int level =0, GLenum target = GL_DRAW_FRAMEBUFFER );
	void update();
	void doColorCorrection(CG2Texture* tex_in, CG2Texture *tex_out);
	void doBoxBlur(CG2Texture *tex_in, CG2Texture* tex_out);
	void doGaussBlur(CG2Texture *tex_in, CG2Texture* tex_interim = nullptr, const int lvl = 0);
	void doGaussBlur4DoF(CG2Texture *tex_in);
	void doDoFEffect(CG2Texture *tex_in,
	                 CG2Texture *tex_depth,
	                 CG2Texture *tex_out = nullptr);
};


#endif // U08POSTRPOCESSING_H
