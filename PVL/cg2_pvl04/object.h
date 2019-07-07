#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "geometry.h"
#include "material.h"
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
	CG2Material* material;

	CG2Object(CG2Geometry* geo=nullptr);
	~CG2Object();
	void createGLObjects();
	void destroyGLObjects();
	void upload();

	void render(CG2BaseVtxShader bvtx,
	            CG2BaseFrgShader bfrg );

	void setModelMatrix(const glm::mat4& m);
	void rotateModelMatrix(const float rad, const float x, const float y, const float z);
	void translateModelMatrix(const float x, const float y, const float z);
	void scaleModelMatrix(const float x, const float y, const float z);
	void scaleModelMatrix(const float s){scaleModelMatrix(s,s,s);}
};

#endif // SCENE_OBJECT_H
