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
	GLint format;

	switch(img.getChannelCount()) {
	case 0:
		warn("The provided texture has no channels, check if it does exist!");
		format=GL_RGB;
		break;
	case 1:
		format=GL_RED;
		break;
	case 2:
		format=GL_RG;
		break;
	case 3:
		format=GL_RGB;
		break;
	case 4:
		format=GL_RGBA;
		break;
	default:
		warn("More than 4 texture channels are not supported!");
		format=GL_RGBA;
		break;
	}

	GLint internal_format = format;

	if(img.isSRGB()) {
		switch (format) {
		case GL_RED:
		case GL_RG:
		case GL_RGB: internal_format  = GL_SRGB; break;
		case GL_RGBA: internal_format  = GL_SRGB_ALPHA;break;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img.getWidth(), img.getHeight(), 0, static_cast<GLenum>(format), img.isHDR()?GL_FLOAT:GL_UNSIGNED_BYTE, img.getPixels());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	info("created 2D texture %u: %dx%d",  tex, img.getWidth(), img.getHeight());
}

void CG2Texture::createEmpty(int w, int h, GLint internal_format,GLenum target)
{
	texture_target = target;
	if(!tex)
		glGenTextures(1, &tex);

	glBindTexture(texture_target, tex);
	glTexImage2D(texture_target, 0, internal_format,
	             w, h, 0,internal_format==GL_DEPTH_COMPONENT?GL_DEPTH_COMPONENT:GL_RED,GL_UNSIGNED_BYTE,nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);

	info("created 2D texture %u: %dx%d",
	     tex, w, h);
}

void CG2Texture::setCubeMapSideFrom(const CG2Image &img, GLenum side, GLfloat maxAnisotropy)
{
	GLenum format;
	switch(img.getChannelCount()) {
	case 0:
		warn("The provided texture has no channels, check if it does exist!");
		format = GL_RGB;
		break;
	case 1:
		format=GL_RED ;
		break;
	case 2:
		format=GL_RG;
		break;
	case 3:
		format=GL_RGB;
		break;
	case 4:
		format=GL_RGBA;
		break;
	default:
		warn("More than 4 texture channels are not supported!");
		format=GL_RGBA;
		break;
	}

	GLint internal_format = static_cast<GLint>(format);

	if(img.isSRGB())
	{
		switch(format){
		case GL_RED:
		case GL_RG:
		case GL_RGB:
			internal_format=GL_SRGB;
			break;
		case GL_RGBA:
			internal_format=GL_SRGB_ALPHA;
			break;
		}
	}

	if(!tex) {
		glGenTextures(1, &tex);
		texture_target = GL_TEXTURE_CUBE_MAP;
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexImage2D(side, 0, internal_format, img.getWidth(), img.getHeight(), 0, format, img.isHDR()?GL_FLOAT:GL_UNSIGNED_BYTE, img.getPixels());
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	info("Added cubemap side to texture %u: %dx%d.", tex, img.getWidth(), img.getHeight());
}

void CG2Texture::bind(unsigned int texture_unit_id)
{
	if(tex) {
		glActiveTexture(GL_TEXTURE0+texture_unit_id);
		glBindTexture(texture_target,tex);
	}
}


