#include "vd.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>


#define VD_WRITE_VAR(x,f) fwrite(&x,1,sizeof (x),f)
#define VD_READ_VAR(x,f) fread(&x,1,sizeof (x),f)

const char* vd_AID_to_string(const vd_AttributeID aid)
{
	switch (aid)
	{
	case VD_POSITION: return "POSITION";
	case VD_NORMAL: return "NORMAL";
	case VD_TANGENT: return "TANGENT";
	case VD_BITANGENT: return "BITANGENT";
	case VD_TEXTURE_COODINATE: return "TEXTURE COORDINATE";
	case VD_COLOR: return "COLOR";
	case VD_BONE_WEIGTHS: return "BONE_WEIGHTS";
	case VD_BONE_IDS: return "BONE_IDS";
	default: return  "unknown";
	}
}

//------------------------------------------------------------------------------

const char* vd_PID_to_string(const vd_PrimitiveID prim)
{
	switch (prim)
	{
	case VD_POINTS: return "POINTS";
	case VD_LINES: return "LINES";
	case VD_LINE_LOOP: return "LINE_LOOP";
	case VD_LINE_STRIP: return "LINE_STRIP";
	case VD_TRIANGLES: return "TRIANGLES";
	case VD_TRIANGLE_STRIP: return "TRIANGLE_STRIP";
	case VD_TRIANGLE_FAN: return "TRIANGLE_FAN";
	default: return  "unknown";
	}
}

//------------------------------------------------------------------------------

const char* vd_TID_to_string(const vd_TypeID type)
{
	switch (type)
	{
	case VD_BYTE: return "BYTE";
	case VD_UBYTE: return "UBYTE";
	case VD_SHORT: return "SHORT";
	case VD_USHORT: return "USHORT";
	case VD_INT: return "INT";
	case VD_UINT: return "UINT";
	case VD_FLOAT: return "FLOAT";
	case VD_HALF_FLOAT: return "HALF_FLOAT";
	case VD_DOUBLE: return "DOUBLE";
	case VD_INT_2_10_10_10_REV: return  "VD_INT_2_10_10_10_REV";
	case VD_UINT_2_10_10_10_REV: return  "VD_UINT_2_10_10_10_REV";

	default: return  "unknown";
	}
}

//------------------------------------------------------------------------------

vd_AttributeMeta* vd_AttribMeta_new()
{
	vd_AttributeMeta* r=(vd_AttributeMeta*)malloc(sizeof(vd_AttributeMeta));
	vd_AttribMeta_set(r,VD_POSITION,VD_FLOAT,0,0,0,NULL,NULL,NULL,NULL,NULL);
	return r;
}

void vd_AttribMeta_free(vd_AttributeMeta* am)
{
	free(am);
}

void vd_AttribMeta_set(vd_AttributeMeta* am,
					   const uint32_t id,
					   const vd_TypeID type,
					   const uint32_t count,
					   const uint32_t normalized, const uint32_t offset,
					   float* const_data,
					   float* scale,
					   float* translation, float *minimum, float *maximum)
{
	uint32_t i;
	assert(am);
	am->attribute_meta_size = 6*sizeof (uint32_t)+5*4*sizeof(float);
	am->ID = id;
	am->type = type;
	am->count = count;
	am->offset = offset;
	for(i = 0; i< 4;i++)
	{
		am->constant_data[i] = 0;
		am->scale[i] = 1;
		am->translation[i] = 0;
		am->minimum[i] = 0;
		am->maximum[i] = 0;
	}

	am->flags = 0;
	if(normalized)
		am->flags |= VD_NORMALIZED;
	if(const_data)
		am->flags |= VD_IS_CONSTANT;
	if(scale)
		am->flags |= VD_IS_SCALED;
	if(translation)
		am->flags |= VD_IS_TRANSLATED;


	for(i = 0; i< count;i++)
	{
		if(const_data)
			am->constant_data[i] = const_data[i];
		if(scale)
			am->scale[i] = scale[i];
		if(translation)
			am->translation[i] = translation[i];
		if(minimum)
			am->minimum[i] = minimum[i];
		if(maximum)
			am->maximum[i] = maximum[i];
	}
}

void vd_AttribMeta_write(const vd_AttributeMeta* am, FILE* f)
{
	assert(am);
	VD_WRITE_VAR(am->attribute_meta_size,f);
	VD_WRITE_VAR(am->ID,f);
	VD_WRITE_VAR(am->type,f);
	VD_WRITE_VAR(am->count,f);

	VD_WRITE_VAR(am->flags,f);
	VD_WRITE_VAR(am->offset,f);
	VD_WRITE_VAR(am->scale,f);
	VD_WRITE_VAR(am->translation,f);
	VD_WRITE_VAR(am->constant_data,f);



	VD_WRITE_VAR(am->minimum,f);
	VD_WRITE_VAR(am->maximum,f);

}

void vd_AttribMeta_read(vd_AttributeMeta* am, FILE* f)
{
	size_t to_read,total_size;
	assert(am);
	VD_READ_VAR(am->attribute_meta_size,f);
	total_size = to_read = am->attribute_meta_size-sizeof(uint32_t);
	to_read -= VD_READ_VAR(am->ID,f);
	to_read -= VD_READ_VAR(am->type,f);
	to_read -= VD_READ_VAR(am->count,f);
	to_read -= VD_READ_VAR(am->flags,f);
	to_read -= VD_READ_VAR(am->offset,f);
	to_read -= VD_READ_VAR(am->scale,f);
	to_read -= VD_READ_VAR(am->translation,f);
	to_read -= VD_READ_VAR(am->constant_data,f);


	to_read -= VD_READ_VAR(am->minimum,f);
	to_read -= VD_READ_VAR(am->maximum,f);


	assert(to_read <= total_size);
	fseek(f,(long)to_read,SEEK_CUR);
}

void vd_AttribMeta_print(vd_AttributeMeta* am)
{
	uint32_t i;
	printf("%s(%d)\n",
		   vd_AID_to_string((vd_AttributeID)am->ID),am->ID);
	printf("\ttype: %s(%d)\n",
		   vd_TID_to_string((vd_TypeID)am->type),am->type);

	printf("\tcount: %d\n",
		   am->count);
	printf("\tflags: %d\n",
		   am->flags);
	printf("\toffset: %d\n",
		   am->offset);

	printf("\tscale: ");
	for(i = 0 ; i< 4;i++)
	{
		printf(" %f",am->scale[i]);
	}
	printf("\n\ttransl.: ");
	for(i = 0 ; i< 4;i++)
	{
		printf(" %f",am->translation[i]);
	}
	printf("\n\tc_data: ");
	for(i = 0 ; i< 4;i++)
	{
		printf(" %f",am->constant_data[i]);
	}
	printf("\n\tmin: ");
	for(i = 0 ; i< 4;i++)
	{
		printf(" %f",am->minimum[i]);
	}

	printf("\n\tmax: ");
	for(i = 0 ; i< 4;i++)
	{
		printf(" %f",am->maximum[i]);
	}
}

//------------------------------------------------------------------------------

vd_Meta* vd_Meta_new()
{
	vd_Meta* m = (vd_Meta*)malloc(sizeof(vd_Meta));
	m->meta_size = 8*sizeof(uint32_t);
	m->primitive = VD_TRIANGLES;
	m->index_type = VD_UINT;
	m->primitive_reset_id = UINT32_MAX;
	m->num_vertices = 0;
	m->num_attribs = 0;
	m->num_indices = 0;
	m->vertex_size = 0;
	m->am = NULL;
	return m;
}

vd_Meta *vd_Meta_cpy(const vd_Meta * o)
{
	vd_Meta* m = vd_Meta_new();
	unsigned int i = 0 ;
	m->primitive = o->primitive;
	m->index_type = o->index_type;
	m->primitive_reset_id = o->primitive_reset_id;
	m->num_vertices = o->num_vertices;
	m->num_indices = o->num_indices;

	m->vertex_size = 0;
	m->num_attribs = 0;
	for(i = 0 ; i <o->num_attribs;i++)
	{
		vd_Meta_add_attribute(m,o->am+i);
	}

	return m;
}

void vd_Meta_free(vd_Meta* m)
{
	assert(m);
	free(m->am);
	m->am=NULL;
	free(m);
}


void vd_Meta_add_attribute(vd_Meta* m,const vd_AttributeMeta* am)
{
	assert(m);
	assert(am);
	m->num_attribs++;
	size_t new_size = m->num_attribs*sizeof(vd_AttributeMeta);
	m->am = (vd_AttributeMeta*)realloc(m->am,new_size);
	memcpy(m->am+m->num_attribs-1,am,sizeof(vd_AttributeMeta));

	m->vertex_size+= (uint32_t)vd_AttribMeta_size(am);
}

void vd_Meta_reeval_attributes(vd_Meta*m)
{
	uint32_t i = 0;
	m->vertex_size = 0;
	for(i  = 0; i < m->num_attribs; i++)
	{
		// The packed formats must represent four values so make sure that
		// there are 4 of them
		if(m->am[i].type == VD_INT_2_10_10_10_REV ||
		   m->am[i].type == VD_UINT_2_10_10_10_REV)
			m->am[i].count =4;
		m->am[i].offset = m->vertex_size;
		m->vertex_size+= (uint32_t)vd_AttribMeta_size(m->am+i);

	}
}

uint32_t vd_Meta_get_index_data_size(const vd_Meta* m)
{
	assert(m);
	if(m->index_type == VD_UBYTE)
		return  1*m->num_indices;
	else if(m->index_type == VD_USHORT)
		return  2*m->num_indices;
	return  4*m->num_indices;
}


uint32_t vd_Meta_get_vertex_data_size(const vd_Meta* m)
{
	assert(m);

	return  m->num_vertices*m->vertex_size;
}


void vd_Meta_write(const vd_Meta* m, FILE* f)
{
	uint32_t i;
	assert(m);
	if(!f)
		return;
	VD_WRITE_VAR(m->meta_size,f);
	VD_WRITE_VAR(m->primitive,f);
	VD_WRITE_VAR(m->index_type,f);
	VD_WRITE_VAR(m->primitive_reset_id,f);
	VD_WRITE_VAR(m->num_vertices,f);
	VD_WRITE_VAR(m->num_indices,f);
	VD_WRITE_VAR(m->num_attribs,f);
	VD_WRITE_VAR(m->vertex_size,f);
	for(i = 0; i< m->num_attribs;i++)
		vd_AttribMeta_write(m->am+i,f);
}

void vd_Meta_read(vd_Meta* m, FILE* f)
{
	uint32_t i;
	size_t to_read, total_size;
	assert(m);
	assert(f);

	VD_READ_VAR(m->meta_size,f);
	total_size = to_read = m->meta_size-sizeof(uint32_t);
	to_read -= VD_READ_VAR(m->primitive,f);
	to_read -= VD_READ_VAR(m->index_type,f);
	to_read -= VD_READ_VAR(m->primitive_reset_id,f);
	to_read -= VD_READ_VAR(m->num_vertices,f);
	to_read -= VD_READ_VAR(m->num_indices,f);
	to_read -= VD_READ_VAR(m->num_attribs,f);
	to_read -= VD_READ_VAR(m->vertex_size,f);

	assert(to_read<=total_size);
	fseek(f,(long)to_read,SEEK_CUR);

	size_t new_size = m->num_attribs*sizeof(vd_AttributeMeta);
	m->am = (vd_AttributeMeta*) realloc(m->am,new_size);
	for(i = 0; i< m->num_attribs;i++)
		vd_AttribMeta_read(m->am+i,f);
}


void vd_Meta_write_data(const vd_Meta* m,
						const void* index_data, const void* vertex_data,FILE*f)
{
	assert(m);
	assert(f);
	fwrite(index_data,vd_Meta_get_index_data_size(m),1,f);
	fwrite(vertex_data,vd_Meta_get_vertex_data_size(m),1,f);
}

void vd_Meta_read_data(const vd_Meta* m,
					   void* index_data, void* vertex_data,FILE*f)
{
	assert(m);
	assert(index_data);
	assert(vertex_data);
	assert(f);
	fread(index_data,vd_Meta_get_index_data_size(m),1,f);
	fread(vertex_data,vd_Meta_get_vertex_data_size(m),1,f);
}

#undef VD_WRITE_VAR
#undef VD_READ_VAR

size_t vd_AttribMeta_print_attrib(const vd_AttributeMeta * am, const void *data, FILE *f)
{
	uint32_t i=0;
	fprintf(f,"[");
	if(am->type == VD_INT_2_10_10_10_REV || am->type == VD_UINT_2_10_10_10_REV)
	{
		printf("0x%08X",((uint32_t*)data)[0]);
	}
	else
	{

		if(!(am->flags& VD_NORMALIZED))
		{
			for(i = 0 ; i< am->count;i++)
			{
				if(i != 0)
					fprintf(f,",");
				switch (am->type) {
				case VD_BYTE: fprintf(f,"%d",((int8_t*)data)[i]);break;
				case VD_UBYTE: fprintf(f,"%d",((uint8_t*)data)[i]);break;
				case VD_SHORT:  fprintf(f,"%d",((int16_t*)data)[i]);break;
				case VD_USHORT:  fprintf(f,"%d",((uint16_t*)data)[i]);break;
				case VD_UINT:  fprintf(f,"%d",((uint32_t*)data)[i]);break;
				case VD_INT:  fprintf(f,"%d",((int32_t*)data)[i]);break;
				case VD_FLOAT:  fprintf(f,"% 6.3f",((float*)data)[i]);break;
				case VD_DOUBLE:  fprintf(f,"% 6.3f",((double*)data)[i]);break;
				}
			}
		}
		else
		{
			for(i = 0 ; i< am->count;i++)
			{
				if(i != 0)
					fprintf(f,",");
				switch (am->type) {

				case VD_BYTE:   fprintf(f,"  % 4d/127  ",((int8_t*)data)[i]);break;
				case VD_UBYTE:  fprintf(f,"  % 4d/255  ",((uint8_t*)data)[i]);break;
				case VD_SHORT:  fprintf(f,"% 6d/32767",((int16_t*)data)[i]);break;
				case VD_USHORT: fprintf(f,"% 6d/65535",((uint16_t*)data)[i]);break;

				case VD_FLOAT:  fprintf(f,"%f",((float*)data)[i]);break;
				case VD_DOUBLE:  fprintf(f,"%f",((double*)data)[i]);break;
				}
			}
		}

	}
		fprintf(f,"]");
	return vd_AttribMeta_size(am);
}

void vd_Meta_print_data(const vd_Meta *m, void *index_data, void *vertex_data, FILE *f)
{
	uint32_t i= 0;
	uint32_t j= 0;
	fprintf(f,"=================================================================\nindex_data:\n=================================================================\n");

	for(i = 0 ; i< m->num_indices;i++)
	{
		if(i&&(!(i%3)))
		{
			fprintf(f,"\n");
		}
		switch (m->index_type) {
		case VD_UBYTE: fprintf(f,"%d,",((uint8_t*)index_data)[i]);break;
		case VD_USHORT:  fprintf(f,"%d,",((uint16_t*)index_data)[i]);break;
		case VD_UINT:  fprintf(f,"%d,",((uint32_t*)index_data)[i]);break;
		}
	}
	fprintf(f,"\n=================================================================\nvertex_data:\n=================================================================\n");

	const uint8_t* data_ptr = (const uint8_t*)vertex_data;

	for(i = 0; i <m->num_vertices;i++)
	{
		for(j = 0;j<m->num_attribs;j++)
		{
			if(m->am[j].flags& VD_IS_CONSTANT) continue;
			data_ptr+=vd_AttribMeta_print_attrib(&(m->am[j]),data_ptr,f);
		}
		printf("\n");
	}
}

size_t vd_AttribMeta_size(const vd_AttributeMeta* am)
{
	size_t tsize = 0;
	switch (am->type)
	{
	case VD_UBYTE:
	case VD_BYTE: tsize =  sizeof (uint8_t); break;
	case VD_USHORT:
	case VD_HALF_FLOAT:
	case VD_SHORT: tsize =  sizeof (uint16_t);break;

	case VD_INT:
	case VD_UINT:
	case VD_INT_2_10_10_10_REV:
	case VD_UINT_2_10_10_10_REV:		tsize =  sizeof (uint32_t);break;

	case VD_FLOAT: tsize =  sizeof (float);break;
	case VD_DOUBLE: tsize =  sizeof (double);break;
	}

	if(am->type == VD_INT_2_10_10_10_REV || am->type == VD_UINT_2_10_10_10_REV )
	{
		return tsize;
	}

	return am->count * tsize;
}




