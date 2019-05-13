#ifndef SCENE_H
#define SCENE_H

#include "geometry.h"
#include "camera.h"
#include "lights.h"

#include "object.h"

#include <string>
#include <vector>

#include <map>
#include <set>
#include <glm/gtx/transform.hpp>
class CG2Scene{

	// a set of objects that are rendered automatically every frame when
	// render() is called.
	std::set<CG2Object*> rendered_scene_objects;

	// a map of pointers to CG2Scene objects,
	// referenced by a name (string)
	std::map<std::string, CG2Object*> scene_objects;

	// a map of pointers to CG2GLGeometry objects,
	// referenced by a name (string)
	std::map<std::string, CG2Geometry*> geometries;

public:
	CG2GlobalLights global_lights;
	CG2Camera camera;

	// Returns the SceneObject with the given name.
	// If it does not exist it will be created.
	CG2Object* getObject(const std::string name, bool render=true);

	// Returns the Texture with the given name.
	CG2Geometry* getGeometry(const std::string &name);


	// Shortcut to create and assemble an object an placing it in the scene.
	CG2Object* placeObject(const std::string& name,
	                            const std::string& geometry,
	                            const glm::vec3& translate=glm::vec3(0.0f),
	                            const float angle_y_deg=0.0f);

	// This will initialize the camera an global_lights
	void createGLObjects();

	// This will destroy all objects, geometries, lights and cameras
	void destroyGLObjects();

	// This will update camera and lights and then call the render
	// method of every Object in rendered_scene_objects.
	void render();
};


#endif // SCENE_H
