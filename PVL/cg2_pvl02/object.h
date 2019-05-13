#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "geometry.h"

#pragma pack(push,1)
struct uboObjectData
{
	glm::mat4 model_matrix;
	glm::mat4 normal_matrix;
};
#pragma pack(pop)

class CG2Object
{
	uboObjectData data;
	GLuint ubo_object;
	bool dirty_ubo;
public:
	CG2Geometry* geometry;

	CG2Object(CG2Geometry* geo=nullptr);
	~CG2Object();
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
	 * @brief render binds the ubo to the index CG2_UBO_OBJECT_DATA (2)
	 * of GL_UNIFORM_BUFFER and renders the associated geometry.
	 */
	void render();

	/**
	 * @brief destroyGLObjects destorys the ubo
	 */
	void destroyGLObjects();


	void setModelMatrix(const glm::mat4& m);
	void rotateModelMatrix(const float rad, const float x, const float y, const float z);
	void translateModelMatrix(const float x, const float y, const float z);
	void scaleModelMatrix(const float x, const float y, const float z);
	void scaleModelMatrix(const float s){scaleModelMatrix(s,s,s);}
};

#endif // SCENE_OBJECT_H
