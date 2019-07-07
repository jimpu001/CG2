#include "u09shadowmapping.h"

void CG2ShadowMapping::set_camera(int glight_id)
{
	CG2_PUSH_DBG_GROUP("CG2ShadowMapping::set_camera");
	auto& light = scene->global_lights.light(glight_id);

	// PVL 4 - change this part to use a directional light
	// press "up-down-left-right" to move light
	// press "F4" to start anime
	light_cam.setNearFar(0.5f,30.0f);
	light_cam.setViewMatrix(glm::lookAt(glm::vec3(light.position), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
	light_cam.setProjectionMatrix(
		glm::ortho(
			-10.0f,
			10.0f,
			-10.0f,
			10.0f,
			light_cam.getData().near_far_width_height.x,
			light_cam.getData().near_far_width_height.y
		)
	);
	light_cam.use();
	light.worldSpaceToShadowMap = glm::mat4(1.0);

	// projection creates range of -1 to 1 (after division)
	// create bias matrix to transform to range [0,1]

	float z_offset = 0.005f;

	glm::mat4 bias = glm::mat4(
	                        glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
	                        glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
	                        glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
	                        /* XXX bias on z against shadow acne */
	                        glm::vec4(0.5f, 0.5f, 0.5f - z_offset, 1.0f)
	                        );

	light.worldSpaceToShadowMap = bias * light_cam.getData().projection * light_cam.getData().view;
	CG2_POP_DBG_GROUP();
}

CG2ShadowMapping::CG2ShadowMapping():
        width(0),
        height(0),
        fbo(0),
		tex_filter_param(GL_NEAREST)
{
}

void CG2ShadowMapping::createGLObjects(CG2Scene &scene, GLsizei w, GLsizei h)
{
	CG2_PUSH_DBG_GROUP("CG2ShadowMapping::createGLObjects");
	this->scene = &scene;
	width = w;
	height = h;
	light_cam.createGLObjects();
	info("initializing shadow map for %dx%d",
	     static_cast<int>(w),
	     static_cast<int>(h));
	// main framebuffer: depth texture + FBO
	tex_shadow_map.createEmpty(w,h,GL_DEPTH_COMPONENT);
	tex_shadow_map.bind(CG2_TEXTURE_UNIT_SHADOW_MAP);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_filter_param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_filter_param);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_shadow_map.getGLName(), 0);
	glDrawBuffer(GL_NONE);
	GLenum status;
	if ( (status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE ) {
		warn("shadow map: incomplete FBO, code 0x%x",static_cast<unsigned>(status));
	}

	CG2_POP_DBG_GROUP();
}

void CG2ShadowMapping::renderShadowMap()
{
	CG2_PUSH_DBG_GROUP("CG2ShadowMapping::renderShadowMap");	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
	set_camera(0);
	glDepthFunc(GL_LEQUAL);
	scene->render(BVTX_POSITION_ONLY,BFRG_NULL);
	tex_shadow_map.bind(CG2_TEXTURE_UNIT_SHADOW_MAP);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	CG2_POP_DBG_GROUP();
}

void CG2ShadowMapping::destroyGLObjects()
{
	CG2_PUSH_DBG_GROUP("CG2ShadowMapping::destroyGLObjects");
	tex_shadow_map.destroyGLObjects();
	light_cam.destroyGLObjects();
	CG2_POP_DBG_GROUP();
}

void CG2ShadowMapping::resizeShadowMap(GLsizei w, GLsizei h)
{
	CG2_PUSH_DBG_GROUP("CG2ShadowMapping::destroyGLObjects");
	tex_shadow_map.destroyGLObjects();
	light_cam.destroyGLObjects();
	CG2_POP_DBG_GROUP();
	
	createGLObjects(*scene, w, h);
}

void CG2ShadowMapping::changeTextureFilter()
{
	tex_shadow_map.bind(CG2_TEXTURE_UNIT_SHADOW_MAP);
	tex_filter_param = tex_filter_param == GL_NEAREST ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_filter_param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_filter_param);	
}