#include "lights.h"

CG2GlobalLights::CG2GlobalLights()
{
	ubo_lights = 0;
	for(int i = 0; i< CG2_MAX_GLOBAL_LIGHTS; i++)
		dirty[i] = true;
}

CG2GlobalLights::~CG2GlobalLights()
{
	destroyGLObjects();
}

void CG2GlobalLights::createGLObjects()
{
	if(ubo_lights)
		return;
	glGenBuffers(1, &ubo_lights);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_lights);
	glBufferData(GL_UNIFORM_BUFFER,sizeof (uboLightData)*CG2_MAX_GLOBAL_LIGHTS,lights,GL_STATIC_DRAW);
	upload();
	use();
}

void CG2GlobalLights::upload()
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_lights);
	for(unsigned int  i = 0 ; i< CG2_MAX_GLOBAL_LIGHTS; i++){
		if(dirty[i]){
			glBufferSubData(GL_UNIFORM_BUFFER,i*sizeof (uboLightData),
			                sizeof (uboLightData),lights+i);
			dirty[i] = false;
		}
	}
}

void CG2GlobalLights::use()
{
	upload();
	glBindBufferBase(GL_UNIFORM_BUFFER, CG2_UBO_GLOBAL_LIGHT_DATA, ubo_lights);
}

void CG2GlobalLights::destroyGLObjects()
{
	if (ubo_lights)	{
		glDeleteBuffers(1, &ubo_lights);
		ubo_lights = 0;
	}
}
