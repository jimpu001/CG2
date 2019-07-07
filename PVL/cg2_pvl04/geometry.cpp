#include "geometry.h"
#include "util.h"
#include "vd/vd.h"
/****************************************************************************
 * CG2GeometryObject                                                      *
 ****************************************************************************/

CG2Geometry::CG2Geometry()
{
	vao = 0;
	vbo = 0;
	index_count = 0;
	primitive_mode = GL_TRIANGLES;
	index_type = GL_UNSIGNED_INT;
	name.clear();
}

CG2Geometry::~CG2Geometry()
{
	destroyGLObjects();
}

void CG2Geometry::destroyGLObjects()
{
	if (vbo) {
		glDeleteBuffers(1, &vbo);
		vbo=0;
	}
	if (ibo) {
		glDeleteBuffers(1, &ibo);
		ibo=0;
	}
	if (vao) {
		glDeleteVertexArrays(1, &vao);
		vao=0;
	}
	index_count=0;
}

void CG2Geometry::load(const std::string &path)
{
	CG2VertexData vd;

	if(!vd.read(path)){
		warn("Cannot open file %s to load geometry data.",path.c_str());
		return;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	const intptr_t isize = vd.index_data_size();
	const uint32_t vsize = vd.vertex_data_size();

	glBufferData(GL_ARRAY_BUFFER,vsize,vd.vertex_data,GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,isize,vd.index_data,GL_STATIC_DRAW);

	const auto& md = vd.meta_data;
	for(auto& a : vd.meta_data.attributes){
		if(!a.as_integer)
			glVertexAttribPointer(a.attib_id,
			                      static_cast<GLint>(a.count),
			                      a.type,
			                      static_cast<GLboolean>(a.normalized),
			                      static_cast<GLsizei>(md.vertex_size),
			                      reinterpret_cast<void*>(a.offset));
		else {

			glVertexAttribIPointer(a.attib_id,
			                       static_cast<GLint>(a.count),
			                       a.type,
			                       static_cast<GLsizei>(md.vertex_size),
			                       reinterpret_cast<void*>(a.offset));
		}
		glEnableVertexAttribArray(a.attib_id);
	}

	this->index_count =static_cast<GLsizei>(md.num_indices);
	this->primitive_mode = md.primitive;
	this->index_type = md.index_type;
	info("Loaded %s with %d vertices and %d indices",
	     path.c_str(),
	     vd.meta_data.num_vertices,
	     vd.meta_data.num_indices);
}

void CG2Geometry::createEmpty(GLenum primitive, unsigned int num_verts)
{
	destroyGLObjects();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	this->index_count = static_cast<GLsizei>(num_verts);
	this->primitive_mode = primitive;
	this->index_type = GL_NONE;
	this->vbo = 0;

}


void CG2Geometry::render() const
{
	if (!vao)
		return; // abort if the object was not properly initialized

	glBindVertexArray(vao);

	if (index_type != GL_NONE)
		glDrawElements(primitive_mode, index_count, index_type, nullptr);
	else
		glDrawArrays(primitive_mode, 0, index_count);
}

void CG2Geometry::renderInstanced(GLsizei count) const
{
	if (!vao)
		return; // abort if the object was not properly initialized

	glBindVertexArray(vao);

	if(index_type != GL_NONE)
		glDrawElementsInstanced(primitive_mode, index_count, index_type, nullptr, count);
	else
		glDrawArraysInstanced(primitive_mode, 0, index_count, count);
}
