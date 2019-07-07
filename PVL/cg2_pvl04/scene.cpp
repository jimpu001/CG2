#include "scene.h"
CG2Object*CG2Scene::getObject(const std::string name, bool render)
{
	if(scene_objects.find(name) == scene_objects.end()){
		scene_objects[name] = new CG2Object();
		scene_objects[name]->createGLObjects();
		if(render){
			rendered_scene_objects.push_back(scene_objects[name]);
		}
	}
	return scene_objects[name];
}

CG2Geometry* CG2Scene::getGeometry(const std::string& name)
{
	if(geometries.find(name) == geometries.end())
		geometries[name] = new CG2Geometry();
	return geometries[name];
}

CG2Texture *CG2Scene::getTexture(const std::string &name)
{
	if(textures.find(name) == textures.end())
		textures[name] = new CG2Texture();
	return textures[name];
}

CG2Material *CG2Scene::getMaterial(const std::string &name)
{
	if(materials.find(name) == materials.end())
		materials[name] = new CG2Material();
	return materials[name];
}

CG2ShaderProgram* CG2Scene::getProgram(const std::string &name)
{
	auto it = programs.find(name);
	if(it == programs.end())
		return nullptr;
	return it->second;
}

CG2ShaderProgram* CG2Scene::buildShaderProgramm(const std::string name, const std::vector<std::string> &paths, bool link_to_base_shaders)
{
	CG2ShaderProgram* sp = shader_comp.build_program(paths,link_to_base_shaders);
	programs[name] = sp;
	return sp;
}

CG2Object*CG2Scene::placeObject(const std::string& name, const std::string& geometry,const std::string& material,const glm::vec3& translate, const float angle_y_deg)
{
	CG2Object* obj= getObject(name);
	obj->geometry = getGeometry(geometry);
	obj->material = getMaterial(material);
	obj->setModelMatrix( glm::translate(translate)*
	                     glm::rotate(glm::radians(angle_y_deg),glm::vec3(0,1,0)));
	return  obj;
}

void CG2Scene::createGLObjects(){
	global_lights.createGLObjects();
	shader_comp.init();
}

void CG2Scene::destroyGLObjects(){
	global_lights.destroyGLObjects();

	for (auto& so:scene_objects){
		delete so.second;
	}

	for (auto& go:geometries){
		delete go.second;
	}

	rendered_scene_objects.clear();
	scene_objects.clear();
	shader_comp.clear();

	geometries.clear();
}

void CG2Scene::render(CG2BaseVtxShader bvtx,
                      CG2BaseFrgShader bfrg)
{
	CG2_PUSH_DBG_GROUP("CG2Scene::render");
	global_lights.use();	
	for(auto& o : rendered_scene_objects)
		o->render(bvtx,bfrg);
	CG2_POP_DBG_GROUP();
}

