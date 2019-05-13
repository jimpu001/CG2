#ifndef GEOMETRY_OBJECT_H
#define GEOMETRY_OBJECT_H
#include <glad/glad.h>
#include <string>


#include "glm/mat4x4.hpp"
#include "vd/vd.h"


// represent the geometry of a renderable object
// (stored in the GL context)
class CG2Geometry {
public:
	GLuint vao;
	GLuint vbo;
	GLuint ibo;

	GLsizei index_count;
	GLenum primitive_mode;
	GLenum index_type;
	std::string name;

	CG2Geometry();
	~CG2Geometry();
	void destroyGLObjects();

	void load(const std::string& mesh);
	void createEmpty(GLenum primitive, unsigned int num_verts);

	void render() const;
	void renderInstanced(GLsizei count) const;
	void setName(const std::string& n) {name=n;}
	std::string getName(const std::string& str) {return name+": "+str;}
};



#endif // GEOMETRY_OBJECT_H
