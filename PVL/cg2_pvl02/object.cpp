#include "object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "bindings_locations.h"

void CG2Object::setModelMatrix(const glm::mat4 &m)
{
	data.model_matrix = m;
	dirty_ubo = true;
}

void CG2Object::rotateModelMatrix(const float rad, const float x, const float y, const float z)
{
	data.model_matrix = glm::rotate(data.model_matrix,rad,glm::vec3(x,y,z));
	dirty_ubo = true;
}

void CG2Object::translateModelMatrix(const float x, const float y, const float z)
{
	data.model_matrix = glm::translate(data.model_matrix,glm::vec3(x,y,z));
	dirty_ubo = true;
}

void CG2Object::scaleModelMatrix(const float x, const float y, const float z)
{
	data.model_matrix = glm::scale(data.model_matrix,glm::vec3(x,y,z));
	dirty_ubo = true;
}

CG2Object::CG2Object(CG2Geometry* geo)
        :geometry(geo)
{
	ubo_object = 0;
	data.model_matrix=glm::mat4(1.0f);
	data.normal_matrix = glm::transpose(glm::inverse(data.model_matrix));
}

CG2Object::~CG2Object()
{
	destroyGLObjects();
}

/**
 * @brief createGLObjects generates the ubo
 */
void CG2Object::createGLObjects()
{
	if (ubo_object)
		return;
	glGenBuffers(1, &ubo_object);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_object);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(data), &data, GL_STATIC_DRAW);

	upload();
}

/**
 * @brief upload updates the data stored in the ubo, but only if the
 * ubo is marked dirty with the 'dirty_ubo' flag
 */
void CG2Object::upload()
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_object);

	if (dirty_ubo) {
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
		dirty_ubo = false;
	}
}

/**
 * @brief render binds the ubo to the index CG2_UBO_OBJECT_DATA (2)
 * of GL_UNIFORM_BUFFER and renders the associated geometry.
 */
void CG2Object::render()
{
	upload();
	glBindBufferBase(GL_UNIFORM_BUFFER, CG2_UBO_OBJECT_DATA, ubo_object);
	geometry->render();
}

void CG2Object::destroyGLObjects()
{
	if(ubo_object) {
		glDeleteBuffers(1,&ubo_object);
	}
}
