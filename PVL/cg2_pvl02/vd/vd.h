#ifndef VD_DATA_H
#define VD_DATA_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
enum CG2VDTypeID: uint32_t
{
	CG2_BYTE =0x1400,            // == GL_BYTE
	CG2_UBYTE =0x1401,           // == GL_UNSIGNED_BYTE
	CG2_SHORT =0x1402,           // == GL_SHORT
	CG2_USHORT =0x1403,          // == GL_UNSIGNED_SHORT
	CG2_INT =0x1404,             // == GL_INT
	CG2_UINT =0x1405,            // == GL_UNSIGNED_INT
	CG2_HALF_FLOAT =0x140B,      // == GL_HALF_FLOAT
	CG2_FLOAT =0x1406,           // == GL_FLOAT
	CG2_DOUBLE=0x140A,           // == GL_DOUBLE
	CG2_INT_2_10_10_10_REV = 0x8D9F, // == GL_INT_2_10_10_10_REV
	CG2_UINT_2_10_10_10_REV = 0x8368 // == GL_UNSIGNED_INT_2_10_10_10_REV
} ;

//------------------------------------------------------------------------------
enum CG2VDPrimitiveID: uint32_t
{
	CG2_POINTS = 0x0000,        // == GL_POINTS
	CG2_LINES = 0x0001,         // == GL_LINES
	CG2_LINE_LOOP = 0x0002,     // == GL_LINE_LOOP
	CG2_LINE_STRIP = 0x0003,    // == GL_LINE_STRIP
	CG2_TRIANGLES = 0x0004,     // == GL_TRIANGLES
	CG2_TRIANGLE_STRIP = 0x0005,// == GL_TRIANGLE_STRIP
	CG2_TRIANGLE_FAN = 0x0006   // == GL_TRIANGLE_FAN
};
//------------------------------------------------------------------------------

enum CG2VDAttributeID : uint32_t
{
	CG2_POSITION =0x0000,
	CG2_NORMAL ,
	CG2_TANGENT,
	CG2_BITANGENT,
	CG2_TEXTURE_COODINATE,
	CG2_COLOR,
	CG2_BONE_WEIGTHS,
	CG2_BONE_IDS,
	CG2_BONE_COUNT,
	CG2_LAST = CG2_BONE_COUNT,
};

//------------------------------------------------------------------------------
/**
 * @brief The CG2VDAttribute struct holds the Attribute configuration
 * for one attribute.
 */
struct CG2VDAttribute
{
	uint32_t internal_attrib_size; // Ignore this !!!
	uint32_t attib_id;   // id of this attribute.
	uint32_t type;       // datatype of this attribute (one of CG2VDTypeID)
	uint32_t count;      // number of elements this attribute has (one of [1,4])
	uint32_t normalized; // is the attribute normalized
	uint32_t as_integer; // is this attribute ment to be an integer
	uint32_t offset;     // offset from the beginning of the vertex in bytes
	CG2VDAttribute();
};


/**
 * @brief The CG2VDMeta struct holds the meta data for a mesh, including the
 * CG2VDAttribute data, the number of indices, attibutes, information about the
 * index data.
 */
struct CG2VDMeta
{
	uint32_t internal_meta_size; // Ignore this !!!
	uint32_t primitive;   // the primitive type used to render this model
	uint32_t index_type;  // the index type (one of  UBYTE,USHORT or UINT)
	uint32_t num_vertices;// number of vertices in the vertex array
	uint32_t num_indices; // number of indeces in the index array
	uint32_t vertex_size; // size of a single vertex (stride) in bytes
	uint32_t num_attribs; // number of attributes (attributes.size())
	std::vector<CG2VDAttribute> attributes;
	CG2VDMeta();
};

/**
 * @brief The CG2VertexData struct contains the data read from a .cg2vd file.
 * You can reuse this structure to load another file by calling read() again.
 */
struct CG2VertexData
{
	// The meta information of the vertex data
	CG2VDMeta meta_data;
	// The index array
	void* index_data;
	// The interleaved vertex array
	void* vertex_data;
	CG2VertexData();
	~CG2VertexData();
	/**
	 * @brief read a .cg2vd file into this object.
	 * @param path to the .cg2vd file.
	 * @return true if the file was read, false if there was an error.
	 */
	bool read(const std::string& path);
	/**
	 * @brief write this Object back to the filesystem.
	 * @param path to where the file should be written to.
	 * @return true if everything went well, false if there was a problem.
	 */
	bool write(const std::string& path);

	/**
	 * @brief index_data_size returns the size of the index data
	 * @return size in bytes
	 */
	uint32_t index_data_size()const;
	/**
	 * @brief vertex_data_size gives the size of the vertex data
	 * @return size in bytes
	 */
	uint32_t vertex_data_size()const;
};


#endif //VD_DATA_H
