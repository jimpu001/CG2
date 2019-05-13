#include "geometry.h"
#include "util.h"
#include "vd/vd.h"
/****************************************************************************
 * CG2GeometryObject                                                      *
 ****************************************************************************/

CG2Geometry::CG2Geometry()
{
	vao=0;
	vbo=0;
	index_count=0;
	primitive_mode = GL_TRIANGLES;
	index_type = GL_UNSIGNED_INT;
}

CG2Geometry::~CG2Geometry()
{
	destroyGLObjects();
}

void CG2Geometry::destroyGLObjects()
{
	if (vbo) {
		info("destroying Buffer %u", vbo);
		glDeleteBuffers(1, &vbo);
		vbo=0;
	}
	if (ibo) {
		info("destroying Buffer %u", ibo);
		glDeleteBuffers(1, &ibo);
		ibo=0;
	}
	if (vao) {
		info("destroying VAO %u", vao);
		glDeleteVertexArrays(1, &vao);
		vao=0;
	}
	index_count=0;
}

void CG2Geometry::load(const std::string &path)
{
	CG2VertexData vd;

	// Load the vertex data
	// ...
	vd.read(path);
	// Generate vao name and bind it
	// ...
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// generate buffer names
	// ...
	glGenBuffers(1, &vbo);
	glGenBuffers(2, &ibo);
	// bind buffers
	// ...
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// upload the vertex and index data to the corresponding buffers
	// ...
	glBufferData(GL_ARRAY_BUFFER, vd.meta_data.num_vertices*vd.meta_data.vertex_size, vd.vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vd.meta_data.num_indices*sizeof(vd.meta_data.index_type), vd.index_data, GL_STATIC_DRAW);
	// iterate through the vd.meta_data.attributes and set all the Vertex Attribute Pointers
	for(const auto& a : vd.meta_data.attributes){
		//...
		glVertexAttribPointer(a.attib_id, a.count, a.type, a.normalized, vd.meta_data.vertex_size, (GLvoid*)a.offset);
		glEnableVertexAttribArray(a.attib_id);
	}

	// store the information needed to initiate a draw call ...
	this->index_count = vd.meta_data.num_indices;
	this->primitive_mode = vd.meta_data.primitive;
	this->index_type = vd.meta_data.index_type;
}


void CG2Geometry::render() const
{
	if (!vao)
		return; // abort if the object was not properly initialized

	// make sure the correct vertex array object is bound
	// ...
	glBindVertexArray(vao);
	// issue the draw call
	glDrawElements(primitive_mode,index_count,index_type,0);

}

