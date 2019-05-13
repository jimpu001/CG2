#include "vd.h"
#include "vd_internals.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>



CG2VDAttribute::CG2VDAttribute()
{
	CG2VDInternals::Attribute_set(*this,
	                              CG2VDAttributeID::CG2_POSITION,
	                              CG2VDTypeID::CG2_FLOAT,
	                              0,0,0,0);

	internal_attrib_size = sizeof(internal_attrib_size)+
	                       sizeof(attib_id)+
	                       sizeof(type)+
	                       sizeof(count)+
	                       sizeof(normalized)+
	                       sizeof(as_integer)+
	                       sizeof(offset);
}

CG2VDMeta::CG2VDMeta(){
	primitive = CG2_TRIANGLES;
	index_type = CG2_UINT;
	num_vertices = 0;
	num_indices = 0;
	vertex_size = 0;

	internal_meta_size = sizeof(internal_meta_size)+
	                     sizeof(primitive)+
	                     sizeof(index_type)+
	                     sizeof(num_vertices)+
	                     sizeof(num_indices)+
	                     sizeof(vertex_size)+
	                     sizeof(num_attribs);

}

// #############################################################################
//       The Internals
// #############################################################################

#define VD_WRITE_VAR(x,f) fwrite(&x,1,sizeof (x),f)
#define VD_READ_VAR(x,f) fread(&x,1,sizeof (x),f)


void CG2VDInternals::CG2VDInternals::CG2VDInternals::Attribute_set(CG2VDAttribute& am,
                                                                   const uint32_t id,
                                                                   const CG2VDTypeID type,
                                                                   const uint32_t count,
                                                                   const uint32_t normalized,
                                                                   const uint32_t as_integer,
                                                                   const uint32_t offset)
{
	am.attib_id = id;
	am.type = type;
	am.count = count;
	am.normalized = normalized;
	am.as_integer= as_integer;
	am.offset = offset;
}

void CG2VDInternals::CG2VDInternals::Attribute_write(const CG2VDAttribute &am, FILE* f)
{

	VD_WRITE_VAR(am.internal_attrib_size,f);
	VD_WRITE_VAR(am.attib_id,f);
	VD_WRITE_VAR(am.type,f);
	VD_WRITE_VAR(am.count,f);

	VD_WRITE_VAR(am.normalized,f);
	VD_WRITE_VAR(am.as_integer,f);
	VD_WRITE_VAR(am.offset,f);

}

void CG2VDInternals::CG2VDInternals::Attribute_read(CG2VDAttribute &am, FILE* f)
{
	size_t to_read,total_size;

	VD_READ_VAR(am.internal_attrib_size,f);
	total_size = to_read = am.internal_attrib_size-sizeof(am.internal_attrib_size);
	to_read -= VD_READ_VAR(am.attib_id,f);
	to_read -= VD_READ_VAR(am.type,f);
	to_read -= VD_READ_VAR(am.count,f);
	to_read -= VD_READ_VAR(am.normalized,f);
	to_read -= VD_READ_VAR(am.as_integer,f);
	to_read -= VD_READ_VAR(am.offset,f);

	assert(to_read <= total_size);
	fseek(f,static_cast<long>(to_read),SEEK_CUR);
}



//------------------------------------------------------------------------------

void CG2VDInternals::Meta_add_attribute(CG2VDMeta &m, const CG2VDAttribute& am)
{
	m.attributes.push_back(am);
	m.num_attribs = static_cast<uint32_t>(m.attributes.size());
	m.vertex_size+= static_cast<uint32_t>(CG2VDInternals::Attribute_size(am));
}

void CG2VDInternals::Meta_reeval_attributes(CG2VDMeta &m)
{
	uint32_t i = 0;
	m.vertex_size = 0;
	for(i  = 0; i < m.attributes.size(); i++){
		// The packed formats must represent four values so make sure that
		// there are 4 of them
		if(m.attributes[i].type == CG2_INT_2_10_10_10_REV ||
		   m.attributes[i].type == CG2_UINT_2_10_10_10_REV)
			m.attributes[i].count =4;
		m.attributes[i].offset = m.vertex_size;
		m.vertex_size+= CG2VDInternals::Attribute_size(m.attributes[i]);
	}
}

uint32_t CG2VDInternals::Meta_get_index_data_size(const CG2VDMeta &m)
{

	if(m.index_type == CG2_UBYTE)
		return  1*m.num_indices;
	else if(m.index_type == CG2_USHORT)
		return  2*m.num_indices;
	return  4*m.num_indices;
}


uint32_t CG2VDInternals::Meta_get_vertex_data_size(const CG2VDMeta m)
{
	return  m.num_vertices*m.vertex_size;
}


void CG2VDInternals::Meta_write(const CG2VDMeta &m, FILE* f)
{

	if(!f)
		return;
	VD_WRITE_VAR(m.internal_meta_size,f);
	VD_WRITE_VAR(m.primitive,f);
	VD_WRITE_VAR(m.index_type,f);
	VD_WRITE_VAR(m.num_vertices,f);
	VD_WRITE_VAR(m.num_indices,f);
	uint32_t n_attribs = static_cast<uint32_t>(m.attributes.size());
	VD_WRITE_VAR(m.vertex_size,f);
	VD_WRITE_VAR(n_attribs,f);
	for(uint32_t i = 0; i< n_attribs;i++)
		CG2VDInternals::CG2VDInternals::Attribute_write(m.attributes[i],f);
}

void CG2VDInternals::Meta_read(CG2VDMeta &m, FILE* f)
{
	uint32_t i;
	size_t to_read, total_size;
	assert(f);

	VD_READ_VAR(m.internal_meta_size,f);
	total_size = to_read = m.internal_meta_size-sizeof(uint32_t);
	to_read -= VD_READ_VAR(m.primitive,f);
	to_read -= VD_READ_VAR(m.index_type,f);
	to_read -= VD_READ_VAR(m.num_vertices,f);
	to_read -= VD_READ_VAR(m.num_indices,f);
	uint32_t n_attribs = 0;
	to_read -= VD_READ_VAR(m.vertex_size,f);
	to_read -= VD_READ_VAR(n_attribs,f);
	assert(to_read<=total_size);
	fseek(f,static_cast<long>(to_read),SEEK_CUR);

	m.attributes.clear();
	for(i = 0; i< n_attribs;i++){
		m.attributes.emplace_back();
		CG2VDInternals::CG2VDInternals::Attribute_read(m.attributes.back(),f);
	}
}

#undef VD_WRITE_VAR
#undef VD_READ_VAR

void CG2VDInternals::Meta_write_data(const CG2VDMeta& m,
                                     const void* index_data, const void* vertex_data,FILE*f)
{
	assert(f);
	fwrite(index_data,CG2VDInternals::Meta_get_index_data_size(m),1,f);
	fwrite(vertex_data,CG2VDInternals::Meta_get_vertex_data_size(m),1,f);
}

void CG2VDInternals::Meta_read_data(const CG2VDMeta &m,
                                    void* index_data, void* vertex_data, FILE*f)
{
	assert(index_data);
	assert(vertex_data);
	assert(f);
	fread(index_data,CG2VDInternals::Meta_get_index_data_size(m),1,f);
	fread(vertex_data,CG2VDInternals::Meta_get_vertex_data_size(m),1,f);
}





uint32_t CG2VDInternals::Attribute_size(const CG2VDAttribute &am)
{
	uint32_t tsize = 0;
	switch (am.type){
	case CG2_UBYTE:
	case CG2_BYTE: tsize =  sizeof(uint8_t); break;
	case CG2_USHORT:
	case CG2_HALF_FLOAT:
	case CG2_SHORT: tsize =  sizeof(uint16_t); break;

	case CG2_INT:
	case CG2_UINT:
	case CG2_INT_2_10_10_10_REV:
	case CG2_UINT_2_10_10_10_REV: tsize =  sizeof (uint32_t);break;

	case CG2_FLOAT: tsize =  sizeof (float);break;
	case CG2_DOUBLE: tsize =  sizeof (double);break;
	}

	if(am.type == CG2_INT_2_10_10_10_REV || am.type == CG2_UINT_2_10_10_10_REV )
		return tsize;


	return am.count * tsize;
}






CG2VertexData::CG2VertexData(){index_data = vertex_data = nullptr;}

CG2VertexData::~CG2VertexData(){
	free(index_data);
	index_data= nullptr;
	free(vertex_data);
	vertex_data = nullptr;
}

bool CG2VertexData::read(const std::string& path)
{
	FILE* f = fopen(path.c_str(),"rb");
	if(!f)
		return false;
	meta_data.attributes.clear();

	CG2VDInternals::Meta_read(meta_data,f);
	index_data = realloc(index_data,
	                     CG2VDInternals::Meta_get_index_data_size(meta_data));
	vertex_data = realloc(vertex_data,
	                      CG2VDInternals::Meta_get_vertex_data_size(meta_data));
	CG2VDInternals::Meta_read_data(meta_data,index_data,vertex_data,f);
	fclose(f);
	return  true;
}

bool CG2VertexData::write(const std::string& path)
{
	FILE* f = fopen(path.c_str(),"wb");
	if(!f || !index_data || !vertex_data)
		return false;
	CG2VDInternals::Meta_write(meta_data,f);
	CG2VDInternals::Meta_write_data(meta_data,index_data,vertex_data,f);
	fclose(f);
	return  true;
}

uint32_t CG2VertexData::index_data_size() const
{
	auto to_ret = CG2VDInternals::Meta_get_index_data_size(meta_data);
	return to_ret;
}

uint32_t CG2VertexData::vertex_data_size() const
{
	auto to_ret = CG2VDInternals::Meta_get_vertex_data_size(meta_data);
	return to_ret;
}
