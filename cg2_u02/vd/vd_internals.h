#ifndef VD_INTERNALS_H
#define VD_INTERNALS_H
#include "vd.h"


class CG2VDInternals
{
public:
	/**
	 * @brief Attribute_size gives the size of one instace of this attribute
	 * @param am
	 * @return
	 */
	static uint32_t Attribute_size(const CG2VDAttribute& am);

	/**
	 * @brief Attribute_set sets content and flags for the given
	 * AttributeMeta.
	 * @param am struct to modify
	 * @param ID the attribute id
	 * @param type the type id of the attribute's values
	 * @param count number of elements (1-4)
	 * @param normalized are the values normalized integers?
	 */
	static void Attribute_set(CG2VDAttribute& am,
	                          const uint32_t id,
	                          const CG2VDTypeID type,
	                          const uint32_t count,
	                          const uint32_t normalized,
	                          const uint32_t as_integer,
	                          const uint32_t offset);

	/**
	 * @brief Attribute_write writes the AttributesMeta data to a
	 * FILE* f.
	 * @param am
	 * @param f
	 */
	static void Attribute_write(const CG2VDAttribute& am, FILE* f);

	/**
	 * @brief Attribute_read reades AttributeMeta from a file.
	 * @param am A previous allocated AttributeMeta
	 * @param f
	 */
	static void Attribute_read(CG2VDAttribute& am, FILE* f);


	/**
	 * @brief Meta_add_attribute adds an Attribute to the Meta. This is
	 * done by copy, so you can destroy am afterwards. The vertex_size is
	 * also increased  by the size of the attribute data.
	 * @param m
	 * @param am
	 */
	static void Meta_add_attribute(CG2VDMeta& m,
	                               const CG2VDAttribute& am);


	/**
	 * @brief Meta_reeval_attributes will check all attributes and
	 * recalculate all offsets as well as the vertex size to match the used
	 * types and counts.
	 * @param m
	 */
	static void Meta_reeval_attributes(CG2VDMeta& m);

	/**
	 * @brief Meta_get_index_data_size returns the size of the
	 * index buffer in bytes.
	 * @param m
	 * @return
	 */
	static uint32_t Meta_get_index_data_size(const CG2VDMeta& m);

	/**
	 * @brief Meta_get_vertex_data_size returns the size of the vertex
	 * buffer in bytes
	 * @param m
	 * @return
	 */
	static uint32_t Meta_get_vertex_data_size(const CG2VDMeta m);

	/**
	 * @brief Meta_write wrties a meta to file, together with all its
	 * attributes.
	 * @param m
	 * @param f
	 */
	static void Meta_write(const CG2VDMeta& m, FILE* f);
	/**
	 * @brief Meta_read reads a metaf from file, together with all its
	 * attributes.
	 * @param m
	 * @param f
	 */
	static void Meta_read(CG2VDMeta& m, FILE* f);

	/**
	 * @brief Meta_write_data writes data (index- and vertex-data) to the
	 * file depending on the information stored in the Meta m. m is _NOT_
	 * written to f!
	 * @param m
	 * @param index_data
	 * @param vertex_data
	 * @param f
	 */
	static void Meta_write_data(const CG2VDMeta& m,
	                            const void* index_data,
	                            const void* vertex_data,
	                            FILE*f);

	/**
	* @brief Meta_read_data reads data (aka index- and vertex-data) from
	* a file depending on the information stored in the Meta m. m is _NOT_
	* read freom f!
	* @param m
	* @param index_data
	* @param vertex_data
	* @param f
	*/
	static void Meta_read_data(const CG2VDMeta& m,
	                           void* index_data,
	                           void* vertex_data,FILE*f);


};

#endif // VD_INTERNALS_H
