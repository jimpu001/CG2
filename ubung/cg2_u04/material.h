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
	CG2ShaderProgram* shader;

	CG2Material(){
		ubo_material=0;
		albedo_map =nullptr;
		shader=nullptr;
	}

	void setAlbedo(const glm::vec4& clr){data.albedo = clr;dirty_ubo=true;}
	void setRoughness(float r){data.material_properties.x = r;dirty_ubo=true;}
	void setMetallic(float m){data.material_properties.y = m;dirty_ubo=true;}
	void setEmission(float e){data.material_properties.z = e;dirty_ubo=true;}


	void createGLObjects(){
		if(ubo_material)
			return;
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
		if(dirty_ubo){
			glBindBuffer(GL_UNIFORM_BUFFER,ubo_material);
			glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof (data),&data);
			dirty_ubo = false;
		}
	}

	void use(CG2BaseVtxShader bvtx,
	         CG2BaseFrgShader bfrg )
	{
		glBindBufferBase(GL_UNIFORM_BUFFER,CG2_UBO_MATERIAL_DATA,ubo_material);
		update();
		shader->use(bvtx,bfrg);
		if(albedo_map)
			albedo_map->bind(CG2_TEXTURE_UNIT_ALBEDO_MAP);
	}
};
#endif // MATERIAL_H
