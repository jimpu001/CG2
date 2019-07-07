#ifndef SCENE_LIGHTS_H
#define SCENE_LIGHTS_H
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "bindings_locations.h"
#define CG2_MAX_GLOBAL_LIGHTS 8 /* you also need to adjust the shaders ... */

#pragma pack(push,1)
struct uboLightData
{
	// position of the light source. If position.w is 0 it is the directon of
	// the resulting directional light source
	// x,y,z : position/direction of the lightsource
	// w     : w=0? -> directional light, w=1-> position of the light
	glm::vec4 position;

	// the color of the light source as well as the ambient factor:
	// x,y,z : Light color
	// w     : Abient factor the ambient color is (x,y,z) * w
	glm::vec4 color_ambientFactor;

	// x,y,z : Spot direction
	// w     : cosine of the cutoff
	glm::vec4 spotDir_cosCutOff;

	// spot exponent, the radius of the light source, an unused value and a
	// flag if the light is enabled.
	// x     : spot exponent
	// y     : radius this lightsource is supposed to influence the scene
	// z     : -- Not used --
	// w     : 1.0 if the light is enabled
	glm::vec4 spotExp_radius_e_enabled;

	// Can be ignored for now! Will be used in shadow mapping
	glm::mat4 worldSpaceToShadowMap;

	uboLightData(){
		setSpotCosCutoff(-2.0);
		setPosition(glm::vec3(0.0f));
		setRadius(-1.0);
		setSpotDirection(glm::vec3(0.0,0.0,-1.0));
		setSpotExp(1);
		setDisabled();
		spotExp_radius_e_enabled.z = 1.0f;
	}
	void setPosition(const glm::vec3& pos){position= glm::vec4(pos,1.0f);}
	void setDirection(const glm::vec3& dir){position= glm::vec4(glm::normalize(dir),0.0f);}
	void setColor(const glm::vec3& color){color_ambientFactor = glm::vec4(color,color_ambientFactor.a);}
	void setSpotDirection(const glm::vec3& dir){spotDir_cosCutOff = glm::vec4(glm::normalize(dir),spotDir_cosCutOff.w);}
	void setSpotCosCutoff(const float& cos_co){spotDir_cosCutOff.a = cos_co;}
	void setSpotExp(const float& e){spotExp_radius_e_enabled.x = e;}	
	void setRadius(const float& r){spotExp_radius_e_enabled.y = r;}
	void setAmbientFactor(const float& af){color_ambientFactor.a = af;}
	void setEnabled(){spotExp_radius_e_enabled.a = 1.0f;}
	void setDisabled(){spotExp_radius_e_enabled.a = 0.0f;}

	bool isEnabled() const {return (spotExp_radius_e_enabled.w > 0.5f);}
	void toggleEnabled() {if (isEnabled()) setDisabled(); else setEnabled();}
	bool isDirectional() const {return (position.w == 0.0f);}
};
#pragma pack(pop)


// The lights affecting the whole scene
class CG2GlobalLights
{
protected:
	GLuint ubo_lights;
	uboLightData lights[CG2_MAX_GLOBAL_LIGHTS];
	bool dirty[CG2_MAX_GLOBAL_LIGHTS];
public:
	const uboLightData& light(int index)const {return lights[index];}
	uboLightData& light(int index){ dirty[index]=true;return lights[index];}
	CG2GlobalLights();
	~CG2GlobalLights();
	void createGLObjects();
	void destroyGLObjects();
	void upload();
	void use();
};



#endif // SCENE_LIGHTS_H
