#ifndef VD_DATA_H
#define VD_DATA_H
#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdio.h>

typedef enum vd_TypeID_t
{
	VD_BYTE =0x1400,            // == GL_BYTE
	VD_UBYTE =0x1401,           // == GL_UNSIGNED_BYTE
	VD_SHORT =0x1402,           // == GL_SHORT
	VD_USHORT =0x1403,          // == GL_UNSIGNED_SHORT
	VD_INT =0x1404,             // == GL_INT
	VD_UINT =0x1405,            // == GL_UNSIGNED_INT
	VD_HALF_FLOAT =0x140B,      // == GL_HALF_FLOAT
	VD_FLOAT =0x1406,           // == GL_FLOAT
	VD_DOUBLE=0x140A,           // == GL_DOUBLE
	VD_INT_2_10_10_10_REV = 0x8D9F, // == GL_INT_2_10_10_10_REV
	VD_UINT_2_10_10_10_REV = 0x8368 // == GL_UNSIGNED_INT_2_10_10_10_REV
} vd_TypeID;
const char* vd_TID_to_string(const vd_TypeID type);

//------------------------------------------------------------------------------
typedef enum vd_PrimitiveID_t
{
	VD_POINTS =0x0000,        // == GL_POINTS
	VD_LINES =0x0001,         // == GL_LINES
	VD_LINE_LOOP =0x0002,     // == GL_LINE_LOOP
	VD_LINE_STRIP =0x0003,    // == GL_LINE_STRIP
	VD_TRIANGLES =0x0004,     // == GL_TRIANGLES
	VD_TRIANGLE_STRIP =0x0005,// == GL_TRIANGLE_STRIP
	VD_TRIANGLE_FAN =0x0006   // == GL_TRIANGLE_FAN
}vd_PrimitiveID;
const char* vd_PID_to_string(const vd_PrimitiveID prim);
//------------------------------------------------------------------------------

typedef enum vd_AttributeID_t
{
	VD_POSITION =0x0000,
	VD_NORMAL ,
	VD_TANGENT,
	VD_BITANGENT,
	VD_TEXTURE_COODINATE,
	VD_COLOR,
	VD_BONE_WEIGTHS,
	VD_BONE_IDS,
	VD_BONE_COUNT,
	VD_LAST = VD_BONE_COUNT,
}vd_AttributeID;

const char* vd_AID_to_string(const vd_AttributeID aid);
//------------------------------------------------------------------------------
typedef enum vd_AttributeFlag_t
{
	VD_NORMALIZED = 1,
	VD_IS_CONSTANT = 2,
	VD_IS_SCALED = 4,
	VD_IS_TRANSLATED=8,
} vd_AttributeFlag;
//------------------------------------------------------------------------------

/**
 * @brief The vd_AttributeMeta struct holds the Attribute configuration
 * for one attribute.
 */
typedef struct vd_AttributeMeta_t
{
	uint32_t attribute_meta_size; // this is stored in the file
	uint32_t ID;     // id of this attribute.
	uint32_t type;   // datatype of this attribute.
	uint32_t count;  // number of elements this attribute has.
	uint32_t flags;  // flags for this attribute.
	uint32_t offset; // offset form the beginning of the vertex in bytes.
	float scale[4];  // scaling for the attribute data.
	float translation[4]; // translation for the attribute data.
	// data*scale+translation = actual_data

	float constant_data[4];// if the IS_CONSTANT flag is set, this is the whole
	// data for the attribute

	float minimum[4]; //per element minimum
	float maximum[4]; //per element maximum
}vd_AttributeMeta;

/**
 * @brief vd_AttribMeta_new creates a new AtributeMeta structure
 * @return
 */
vd_AttributeMeta* vd_AttribMeta_new();

/**
 * @brief vd_AttribMeta_free will free all memory related to an am created with
 * vd_AttribMeta_new.
 * @param am
 */
void vd_AttribMeta_free(vd_AttributeMeta* am);


/**
 * @brief vd_AttribMeta_size gives the size of one instace of this attribute
 * @param am
 * @return
 */
size_t vd_AttribMeta_size(const vd_AttributeMeta* am);

/**
 * @brief vd_AttribMeta_set sets content and flags for the given AttributeMeta.
 * @param am struct to modify
 * @param ID the attribute id
 * @param type the type id of the attribute's values
 * @param count number of elements (1-4)
 * @param normalized are the values normalized integers?
 * @param const_data NULL if the data should not be constant or the constant
 * value in float (constant data is allways float).
 * @param scale the scale factor applied to the attribute,
 * or NULL if no scaling is required.
 * @param translate the translation applied to the attribute, or NULL if none
 * is needed.
 */
void vd_AttribMeta_set(vd_AttributeMeta* am,
					   const uint32_t id,
					   const vd_TypeID type,
					   const uint32_t count,
					   const uint32_t normalized,
					   const uint32_t offset,
					   float* const_data,
					   float* scale,
					   float* translation,
					   float* minimum,
					   float* maximum);

/**
 * @brief vd_AttribMeta_write writes the AttributesMeta data to a FILE* f.
 * @param am
 * @param f
 */
void vd_AttribMeta_write(const vd_AttributeMeta* am, FILE* f);

/**
 * @brief vd_AttribMeta_read reades AttributeMeta from a file.
 * @param am A previous allocated AttributeMeta
 * @param f
 */
void vd_AttribMeta_read(vd_AttributeMeta* am, FILE* f);
void vd_AttribMeta_print(vd_AttributeMeta* am);



//------------------------------------------------------------------------------

/**
 * @brief The vd_Meta struct holds the meta data for a mesh, including the
 * AttributeMeta data, the number of indices, attibutes, information about the
 * index data, such as its type, and the primitive_reset_id.
 */
typedef struct vd_Meta_t
{
	uint32_t meta_size; // size ___without__ the attribute meta data
	uint32_t primitive;
	uint32_t index_type;
	uint32_t primitive_reset_id;
	uint32_t num_vertices;
	uint32_t num_indices;
	uint32_t num_attribs;
	uint32_t vertex_size; // size of a single vertex

	vd_AttributeMeta* am;

}vd_Meta;

/**
 * @brief vd_Meta_new creates a new vd_Meta struct.
 * @return
 */
vd_Meta* vd_Meta_new();

/**
 * @brief vd_Meta_cpy creates a new vd_Meta struct based on an other one.
 * @return
 */
vd_Meta* vd_Meta_cpy(const vd_Meta *m);

/**
 * @brief vd_Meta_free destroys a vd_Meta struct as well as all assoziated
 * vd_AttributeMeta structs.
 * @param m
 */
void vd_Meta_free(vd_Meta* m);

/**
 * @brief vd_Meta_add_attribute adds an Attribute to the Meta. This is done
 * by copy, so you can destroy am afterwards. The vertex_size is also increased
 * by the size of the attribute data.
 * @param m
 * @param am
 */
void vd_Meta_add_attribute(vd_Meta* m,const vd_AttributeMeta* am);


/**
 * @brief vd_Meta_reeval_attributes will check all attributes and recalculate
 * all offsets as well as the vertex size to match the used types and counts.
 * @param m
 */
void vd_Meta_reeval_attributes(vd_Meta*m);

/**
 * @brief vd_Meta_get_index_data_size returns the size of the index buffer in
 * bytes
 * @param m
 * @return
 */
uint32_t vd_Meta_get_index_data_size(const vd_Meta* m);

/**
 * @brief vd_Meta_get_vertex_data_size returns the size of the vertex buffer in
 * bytes
 * @param m
 * @return
 */
uint32_t vd_Meta_get_vertex_data_size(const vd_Meta* m);

/**
 * @brief vd_Meta_write wrties a meta to file, together with all its attributes.
 * @param m
 * @param f
 */
void vd_Meta_write(const vd_Meta* m, FILE* f);
/**
 * @brief vd_Meta_read reads a metaf from file, together with all its attributes.
 * @param m
 * @param f
 */
void vd_Meta_read(vd_Meta* m, FILE* f);

/**
 * @brief vd_Meta_write_data writes data (index- and vertex-data) to the file
 * depending on the information stored in the Meta m. m is _NOT_ written to f!
 * @param m
 * @param index_data
 * @param vertex_data
 * @param f
 */
void vd_Meta_write_data(const vd_Meta* m,
						const void* index_data,
						const void* vertex_data,
						FILE*f);

/**
 * @brief vd_Meta_read_data reads data (aka index- and vertex-data) from a file
 * depending on the information stored in the Meta m. m is _NOT_ read freom f!
 * @param m
 * @param index_data
 * @param vertex_data
 * @param f
 */
void vd_Meta_read_data(const vd_Meta* m,
					   void* index_data,
					   void* vertex_data,FILE*f);

/**
 * @brief vd_Meta_print_data will print the data in a human readable form.
 * Meta will not be printed! just the index data, followed by the vertex data.
 * @param m
 * @param index_data
 * @param vertex_data
 * @param f
 */
void vd_Meta_print_data(const vd_Meta* m,
					   void* index_data,
					   void* vertex_data,FILE*f);



#ifdef __cplusplus
}
#endif

#endif //VD_DATA_H
