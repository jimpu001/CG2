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

	/**
	 * @brief createGLObjects generates the ubo
	 */
	void createGLObjects();

	/**
	 * @brief upload updates the data stored in the ubo, but only if the
	 * ubo is marked dirty with the 'dirty_ubo' flag. The 'dirty_ubo' flag
	 * is set to false in this process.
	 */
	void upload();

	/**
	 * @brief render binds the ubo to the index CG2_UBO_CAMERA_DATA(1)
	 * of GL_UNIFORM_BUFFER.
	 */
	void use();

	/**
	 * @brief destroyGLObjects destorys the ubo
	 */
	void destroyGLObjects();

	/**
	 * @brief tick Simulates the camera movement. This should be called once
	 * a frame.
	 * @param eventData The event data from the current frame.
	 */
	void tick(const CG2EventData& eventData);

	void recalculateProjectionMatrix();
	void recalculateViewMatrix();

	void setFOVY(float fovy_rad);
	void setResolution(const glm::ivec2& res);
	void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

protected:
	float speed, theta,phi;
	glm::vec3 position;

};


#endif // SCENE_CAMERA_H