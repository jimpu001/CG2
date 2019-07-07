#ifndef MATERIAL_H
#define MATERIAL_H
#include "texture.h"
#include "bindings_locations.h"
#include "shader_compositor.h"


#pragma pack(push,1)
        struct uboMaterialData
	{
		// albedo color of the material
		glm::vec4 albedo;
		// the material properties Roughness, Metallicness and Emission. zw are not used.
		glm::vec4 material_properties;

		glm::vec4 material_properties_scale;
		glm::vec4 material_properties_offset;
	};

#pragma pack(pop)



/**
 * @brief The CG2Material class manages textures, and later shaders as well as
 *  other material properties.
 */
class CG2Material
{
	uboMaterialData data;
	GLuint ubo_material;
	bool dirty_ubo;

public:
	CG2Texture* albedo_map;
	CG2Texture* normal_map;
	CG2Texture* material_properies_map;
	CG2ShaderProgram* shader;

	void setAlbedo(const glm::vec4& clr)		{ data.albedo = clr; dirty_ubo = true; }
	void setRoughness(float r)	{ data.material_properties.x = r; dirty_ubo = true; }
	void setMetallic(float m)	{ data.material_properties.y = m; dirty_ubo = true; }
	void setEmission(float e)	{ data.material_properties.z = e; dirty_ubo = true; }

	void setRoughnessScale(float rs){ data.material_properties_scale.x = rs; dirty_ubo = true; }
	void setMetallicScale(float ms)	{ data.material_properties_scale.y = ms; dirty_ubo = true; }
	void setEmissionScale(float es)	{ data.material_properties_scale.z = es; dirty_ubo = true; }

	void setRoughnessOffset(float ro){ data.material_properties_offset.x = ro; dirty_ubo = true; }
	void setMetallicOffset(float mo){ data.material_properties_offset.y = mo; dirty_ubo = true; }
	void setEmissionOffset(float eo){ data.material_properties_offset.z = eo; dirty_ubo = true; }


	CG2Material() :
	        dirty_ubo(true),
	        albedo_map(nullptr),
	        normal_map(nullptr),
	        material_properies_map(nullptr)

	{
		ubo_material = 0;
		setAlbedo(glm::vec4(1.0f));
		setRoughness(0.75f);
		setMetallic(0.5f);
		setEmission(0.0f);
		data.material_properties_scale = glm::vec4(1.0f);
		data.material_properties_offset = glm::vec4(0.0f);

		dirty_ubo=true;
	}
	
	void init(CG2ShaderProgram* shader, CG2Texture* a_map = nullptr, CG2Texture* s_map = nullptr, CG2Texture* n_map = nullptr, const glm::vec4& albedo = glm::vec4(1.0f))
	{
		this->shader = shader;
		this->setAlbedo(albedo);

		this->albedo_map = a_map;
		this->material_properies_map = s_map;
		this->normal_map = n_map;
	}

	void createGLObjects(){
		glGenBuffers(1,&ubo_material);
		glBindBuffer(GL_UNIFORM_BUFFER,ubo_material);
		glBufferData(GL_UNIFORM_BUFFER,sizeof (data),&data,GL_STATIC_DRAW);
	}
	void destroyGLObjects(){
		if(ubo_material){
			glDeleteBuffers(1,&ubo_material);
			ubo_material=0;
		}
	}
	void update(){
		if(!ubo_material)
			createGLObjects();
		if(dirty_ubo){
			glBindBuffer(GL_UNIFORM_BUFFER,ubo_material);
			glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof (data),&data);
			dirty_ubo = false;
		}
	}

	void use(CG2BaseVtxShader bvtx,
	         CG2BaseFrgShader bfrg )
	{
		update();
		glBindBufferBase(GL_UNIFORM_BUFFER,CG2_UBO_MATERIAL_DATA,ubo_material);
		shader->use(bvtx,bfrg);

		if (albedo_map)
			albedo_map->bind(CG2_TEXTURE_UNIT_ALBEDO_MAP);
		if (material_properies_map)
			material_properies_map->bind(CG2_TEXTURE_UNIT_MATERIAL_MAP);
		if (normal_map)
			normal_map->bind(CG2_TEXTURE_UNIT_NORMAL_MAP);
	}
};
#endif // MATERIAL_H
