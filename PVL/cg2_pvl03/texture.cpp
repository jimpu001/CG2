#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include <cmath>
#include "texture.h"
#include "util.h"
CG2Texture::CG2Texture()
{
	tex=0;
	texture_target = GL_TEXTURE_2D;
	//texture_target = GL_TEXTURE_CUBE_MAP;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

CG2Texture::~CG2Texture()
{
	destroyGLObjects();
}

void CG2Texture::destroyGLObjects()
{
	if (tex){
		glDeleteTextures(1,&tex);
		tex=0;
	}
}

void CG2Texture::createFrom(const CG2Image& img, GLfloat maxAnisotropy)
{
	destroyGLObjects();
	// ...
	GLint format;
	switch (img.getChannelCount())
	{
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		warn("more than 4 texture channels are not supported");
		format = GL_RGBA;
	}
	glGenTextures(1, &tex);
	glBindTexture(texture_target, tex);
	glTexImage2D(texture_target, 0, format, img.getWidth(), img.getHeight(), 0, format, GL_UNSIGNED_BYTE, img.getPixels());
	glGenerateMipmap(texture_target);
	glTexParameterf(texture_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	info("created 2D texture %u: %dx%d",
	     tex, img.getWidth(), img.getHeight());
}

void CG2Texture::setCubeMapSideFrom(const CG2Image &img, GLenum side, GLfloat maxAnisotropy)
{
	// ...
	GLenum format;
	//区分不同的颜色管道；
	switch (img.getChannelCount()) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		warn("more than 4 texture channels are not supported");
		format = GL_RGBA;
	}
	if (!tex) {
		glGenTextures(1, &tex);
		texture_target = GL_TEXTURE_CUBE_MAP;
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	}
	glTexImage2D(side, 0, format, img.getWidth(), img.getHeight(), 0, format, GL_UNSIGNED_BYTE, img.getPixels());
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(texture_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	info("Added cubemap side to texture %u: %dx%d.",
	     tex, img.getWidth(), img.getHeight());
}

void CG2Texture::bind(unsigned int texture_unit_id)
{
	if(tex){
		glActiveTexture(GL_TEXTURE0+texture_unit_id);
		glBindTexture(texture_target,tex);
	}
}


