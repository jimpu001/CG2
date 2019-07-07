#include "u08postprocessing.h"

CG2Postprocessing::CG2Postprocessing(){
	pp_effect = CG2_PP_COLOR_CORRECTION;
	ubo_pp = 0;
	fbo = 0;
	dirty_ubo = true;
}

void CG2Postprocessing::updateResolution(int w, int h){
	for(int i  =0 ; i< 2;i++){
		tex_color[i].destroyGLObjects();
		tex_color[i].createEmpty(w,h,GL_SRGB_ALPHA);
	}
	tex_blur.destroyGLObjects();
	tex_blur.createEmpty(w,h,GL_SRGB_ALPHA);

	tex_depth.destroyGLObjects();
	tex_depth.createEmpty(w,h,GL_DEPTH_COMPONENT);
	data.viewport=glm::vec4(0,0,w,h);
	dirty_ubo = true;
}

const uboPostProcessingData &CG2Postprocessing::ppData() const{return data;}

uboPostProcessingData &CG2Postprocessing::ppData() {dirty_ubo=true;return data;}

PostProcessingEffect &CG2Postprocessing::ppEffect() { return pp_effect; }

void CG2Postprocessing::createGLObjects(CG2Scene& scene){
	updateResolution(800,800);
	if(!fbo){
		glGenFramebuffers(1,&fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,tex_depth.getGLName(),1);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	}
	if(!ubo_pp){
		glGenBuffers(1,&ubo_pp);
		glBindBufferBase(GL_UNIFORM_BUFFER,CG2_UBO_POST_PROCESSING_DATA,ubo_pp);
		glBufferData(GL_UNIFORM_BUFFER,sizeof(data),&data,GL_STATIC_DRAW);
		dirty_ubo = false;
	}
	screen_filling_quad.createEmpty(GL_TRIANGLE_STRIP,4);
	color_corection = scene.buildShaderProgramm("pp_cc",{"data/shaders/pp.vert",
	                                                     "data/shaders/pp_color.frag"},false);
	box_blur = scene.buildShaderProgramm("pp_bp",{"data/shaders/pp.vert",
	                                              "data/shaders/pp_box_blur.frag"},false);
	gauss_blur = scene.buildShaderProgramm("pp_bp",{"data/shaders/pp.vert",
	                                                "data/shaders/pp_gaussian_blur_5_1D.frag"},false);
	dof = scene.buildShaderProgramm("pp_bp",{"data/shaders/pp.vert",
	                                         "data/shaders/pp_depth_of_field.frag"},false);
}

void CG2Postprocessing::destroyGLObjects(){
	glDeleteFramebuffers(1,&fbo);
	glDeleteBuffers(1,&ubo_pp);
	for(int i  =0 ; i< 2;i++){
		tex_color[i].destroyGLObjects();
	}
	tex_blur.destroyGLObjects();
	tex_depth.destroyGLObjects();
	screen_filling_quad.destroyGLObjects();
}

void CG2Postprocessing::setupFramebuffer(CG2Texture* target_texture, const int level, GLenum target){
	update();
	if(!target_texture){
		glBindFramebuffer(target,0);
	}else{
		glBindFramebuffer(target,fbo);
		glFramebufferTexture2D(target,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,target_texture->getGLName(),level);
		glFramebufferTexture2D(target,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,tex_depth.getGLName(),level);
	}
	glViewport(static_cast<GLint>(data.viewport.x),
	           static_cast<GLint>(data.viewport.y),
	           static_cast<GLsizei>(data.viewport.z),
	           static_cast<GLsizei>(data.viewport.w));


}

void CG2Postprocessing::update(){
	if(dirty_ubo){
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_pp);
		glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(data),&data);
		dirty_ubo = false;
	}

}

void CG2Postprocessing::doColorCorrection(CG2Texture *tex_in, CG2Texture* tex_out){

	color_corection->use();
	tex_in->bind(0);
	setupFramebuffer(tex_out);
	screen_filling_quad.render();
}

void CG2Postprocessing::doBoxBlur(CG2Texture *tex_in, CG2Texture* tex_out){

	box_blur->use();
	tex_in->bind(0);
	setupFramebuffer(tex_out);
	screen_filling_quad.render();
}

void CG2Postprocessing::doGaussBlur(CG2Texture *tex_in, CG2Texture* tex_interim, const int lvl ){

	// select the program
	gauss_blur->use();

	// select a texture for the interim result if needed
	if(!tex_interim)
		tex_interim = &(this->tex_blur);

	// bind the texture and check for its resolution
	tex_in->bind(0);
	int w=0, h=0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, lvl, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, lvl, GL_TEXTURE_HEIGHT, &h);
	data.viewport=glm::vec4(0,0,w,h);
	glViewport(0,0,w,h);
	// set direction upload ubo and first pass
	data.direction_level_e = glm::vec4(1,0,static_cast<float>(lvl),0);
	dirty_ubo = true;
	update();
	setupFramebuffer(tex_interim,lvl);
	screen_filling_quad.render();

	// set direction upload ubo and second pass
	data.direction_level_e = glm::vec4(0,1,static_cast<float>(lvl),0);
	dirty_ubo = true;
	update();
	tex_interim->bind(0);
	setupFramebuffer(tex_in,lvl);
	screen_filling_quad.render();
}

void CG2Postprocessing::doGaussBlur4DoF(CG2Texture *tex_in)
{
	int maxLevel=static_cast<int>(ceilf(data.focalDistance_focalSize_maxLevel_e.z));

	// generate the necessary mipmap levels
	tex_in->bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel);
	glGenerateMipmap(GL_TEXTURE_2D);

	// save the viewport since doGaussBlur will change it
	glm::vec4 vp_sav = data.viewport;

	// do the gaussian Blur for every needed layer
	for(int i =1 ; i< maxLevel;i++){
		doGaussBlur(tex_in,&tex_blur,i);
	}

	// restore the viewport state
	data.viewport=vp_sav;
	glViewport(static_cast<GLint>(vp_sav.x),
	           static_cast<GLint>(vp_sav.y),
	           static_cast<GLsizei>(vp_sav.z),
	           static_cast<GLsizei>(vp_sav.w));
}

void CG2Postprocessing::doDoFEffect(CG2Texture *tex_in,
                                    CG2Texture *tex_depth,
                                    CG2Texture *tex_out)
{
	// make shure the ubo is up to date
	update();
	// bind the preblured color and the corresponding depth map
	tex_in->bind(0);
	tex_depth->bind(1);
	// setup the framebuffer and do the thing
	setupFramebuffer(tex_out,0);
	dof->use();
	screen_filling_quad.render();
}
