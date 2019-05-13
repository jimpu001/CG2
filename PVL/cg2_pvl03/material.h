#ifndef MATERIAL_H
#define MATERIAL_H
#include "texture.h"
#include "bindings_locations.h"
/**
 * @brief The CG2Material class manages textures, and later shaders as well as
 *  other material properties.
 */
class CG2Material
{
public:
	CG2Texture* albedo_map;

	void use()
	{
		if(albedo_map)
			albedo_map->bind(CG2_TEXTURE_UNIT_ALBEDO_MAP);
	}
};
#endif // MATERIAL_H
