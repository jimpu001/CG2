#ifndef CG2_GLOBJECTS_H
#define CG2_GLOBJECTS_H
#include <string>
#include "image.h"

// represent a GL texture object
class CG2Texture
{
	GLuint tex;
	GLenum texture_target;
public:
	CG2Texture();
	~CG2Texture();
	void destroyGLObjects();

	/**
	 * @brief createFrom creates this texture from a given image
	 * @param img
	 * @param maxAnisotropy the maximum anistropy value.
	 */
	void createFrom(const CG2Image& img, GLfloat maxAnisotropy=8.0f);

	/**
	 * @brief setCubeMapSideFrom creates the texture as a cube map and
	 * uploads the images content to the given side of the cube map.
	 * @param img
	 * @param side
	 * @param maxAnisotropy
	 */
	void setCubeMapSideFrom(const CG2Image &img, GLenum side, GLfloat maxAnisotropy = 8.0f);

	/**
	 * @brief bind binds the texture to the internal target.
	 * @param texture_unit_id The texture unit id (0,1,2,... _NOT_
	 * GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2,...)
	 */
	void bind(unsigned int texture_unit_id);

	/**
	 * @brief operator bool returns true if there is a texture object.
	 */
	operator bool()const{return tex != 0;}
};
#endif // CG2_GLOBJECTS_H
