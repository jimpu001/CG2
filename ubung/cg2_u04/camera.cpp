#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>



CG2Camera::CG2Camera()
{
	data.projection = data.view = glm::mat4(1.0f);
	data.near_far_width_height = glm::vec4(0.1f,60.0f,800.0f,800.0f);

	fovy_rad=glm::radians(45.0f);
	ubo_camera = 0;

	speed=5.0f;
	position=glm::vec3(0.0f, 0.0f, 0.0f);
	phi=0.0f;
	theta=0.0f;


	recalculateViewMatrix();
	recalculateProjectionMatrix();

}

CG2Camera::~CG2Camera()
{
	destroyGLObjects();
}

void CG2Camera::recalculateProjectionMatrix()
{
	data.projection = glm::perspective(fovy_rad,
	                                   data.near_far_width_height.z/
	                                   data.near_far_width_height.w,
	                                   data.near_far_width_height.x,
	                                   data.near_far_width_height.y);
	data.i_projection = glm::inverse(data.projection);
	dirty_ubo = true;
}

void CG2Camera::recalculateViewMatrix()
{
	data.view=glm::rotate(glm::radians(theta), glm::vec3(1.0f, 0.0f, 0.0f)) *
	          glm::rotate(glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f)) *
	          glm::translate(-1.0f*position);

	data.i_view = glm::inverse(data.view);
	dirty_ubo = true;
}


void CG2Camera::setFOVY(float fovy){
	if(fabsf(fovy-this->fovy_rad) > 0.0001f){
		this->fovy_rad = fovy;
		recalculateProjectionMatrix();
	}
}

void CG2Camera::setResolution(const glm::ivec2& res)
{
	glm::ivec2 is = data.getResolution();
	if(is.x != res.x || is.y != res.y){
		data.setResolution(res);
		recalculateProjectionMatrix();
	}
}

void CG2Camera::upload()
{
	if(dirty_ubo){
		glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
		glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(uboCameraData),&data);
		dirty_ubo = false;
	}
}


void CG2Camera::lookAt(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
{
	(void) up;
	position = eye;

	const glm::vec3 d = (center-eye);
	const glm::vec3 nz(0.f,0.f,-1.f);
	glm::vec3 dx = d;
	dx.y = 0;
	dx = glm::normalize(dx);
	phi = ((d.x < 0.0f)?-1.0f:1.0f)*glm::degrees(acosf(glm::dot(dx,nz)));
	const glm::mat3 R = glm::rotate(glm::radians(phi), glm::vec3(0.f,1.f,0.f));
	glm::vec3 dy = R*d; dy.x = 0;
	dy = glm::normalize(dy);
	theta =  ((d.y > 0.0f)?-1.0f:1.0f)*glm::degrees(acosf(glm::dot(dy,nz)));
	recalculateViewMatrix();

}

void CG2Camera::tick(const CG2EventData& eventData)
{
	if(!eventData.cursor_catched)
		return;
	glm::mat4 cam= data.i_view;
	GLfloat delta;
	if (eventData.paused) {
		// hack to be able to move in paused mode:
		// just assume 60 fps(!)
		delta=speed * 1.0f/60.0f;
	} else {
		delta=speed * static_cast<GLfloat>(eventData.delta_time);
	}
	glm::vec3 fwd=delta * glm::vec3(cam * glm::vec4(0.0f,0.0f,-1.0f, 0.0f) );
	glm::vec3 rgt=delta * glm::vec3(cam * glm::vec4(1.0f,0.0f, 0.0f, 0.0f) );

	glm::vec3 up=delta * glm::vec3(cam * glm::vec4(0.0f,1.0f, 0.0f, 0.0f) );
	glm::vec3 dwn=delta * glm::vec3(cam * glm::vec4(0.0f,-1.0f, 0.0f, 0.0f) );

	/* WASD controls */
	if (eventData.key_pressed[GLFW_KEY_W]) {
		position += fwd;
	}
	if (eventData.key_pressed[GLFW_KEY_S]) {
		position -= fwd;
	}
	if (eventData.key_pressed[GLFW_KEY_A]) {
		position -= rgt;
	}
	if (eventData.key_pressed[GLFW_KEY_D]) {
		position += rgt;
	}
	if (eventData.key_pressed[GLFW_KEY_SPACE]) {
		position += up;
	}
	if (eventData.key_pressed[GLFW_KEY_LEFT_CONTROL]) {
		position += dwn;
	}


	/* scale mouse speed so that half the window height is 90 degrees */
	glm::vec2 scaledDelta = (180.0f / static_cast<float>(eventData.framebuffer_size.y))
	                        * eventData.mouse_cursor_delta;

	phi += scaledDelta[0];
	theta -= scaledDelta[1];
	if (theta > 90.0f) {
		theta=90.0f;
	}
	if (theta < -90.0f) {
		theta=-90.0f;
	}
	recalculateViewMatrix();

}


void CG2Camera::createGLObjects(){
	if(ubo_camera)
		return;
	glGenBuffers(1,&ubo_camera);
	glBindBuffer(GL_UNIFORM_BUFFER,ubo_camera);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(uboCameraData),&data,GL_DYNAMIC_DRAW);
	dirty_ubo = false;
}

void CG2Camera::destroyGLObjects()
{
	if(ubo_camera){
		glDeleteBuffers(1,&ubo_camera);
		ubo_camera = 0;
	}
}

void CG2Camera::use() {
	upload();
	glBindBufferBase(GL_UNIFORM_BUFFER, CG2_UBO_CAMERA_DATA, ubo_camera);
}

