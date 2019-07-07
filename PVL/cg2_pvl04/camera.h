#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glad/glad.h>
#include <string>

#include "event_data.h"
#include "bindings_locations.h"

#pragma pack(push,1)
struct uboCameraData{
	glm::mat4 view;
	glm::mat4 i_view;
	glm::mat4 projection;
	glm::mat4 i_projection;
	glm::vec4 near_far_width_height;

	glm::ivec2 getResolution() const {
		return glm::ivec2(roundf(near_far_width_height.z),
		                  roundf(near_far_width_height.w));
	}

	void setResolution(const glm::ivec2& res){
		for(int i = 0; i< 2;i++){
			near_far_width_height[2+i] = static_cast<float>(res[i]);
		}
	}
};
#pragma pack(pop)
class CG2Camera {
	uboCameraData data;
	// Projection parameters
	GLfloat fovy_rad;

	// the UBOs to use, might reference our internal ones, or external ones
	GLuint ubo_camera;
	bool dirty_ubo;

public:

	CG2Camera();
	~CG2Camera();

	void createGLObjects();
	void destroyGLObjects();

	void upload();
	void use();

	void tick(const CG2EventData& eventData);

	void recalculateProjectionMatrix();
	void recalculateViewMatrix();

	void setProjectionMatrix(glm::mat4 proj);
	void setViewMatrix(glm::mat4 view);

	void setFOVY(float fovy_rad);
	void setNearFar(float nearVal,float farVal);
	void setResolution(const glm::ivec2& res);
	void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

	uboCameraData& getData() {dirty_ubo = true;return data;}
	const uboCameraData& getData() const {return data;}

	glm::vec3 pos_es_ws(const glm::vec3& v){return glm::vec3(data.i_view*glm::vec4(v,1.0f));}
	glm::vec3 dir_es_ws(const glm::vec3& v){return glm::vec3(data.i_view*glm::vec4(v,0.0f));}


protected:
	float speed, theta,phi;
	glm::vec3 position;

};


#endif // SCENE_CAMERA_H
