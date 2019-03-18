#pragma once
#include <unordered_map>
#include "GameObject.h"
#include "Camera.h"

class ResourceManager
{
public:
	static Transform* addEntity(Transform* entity);
	static void addShader(ShaderProgram* shader);
	static ShaderProgram* addShader(const std::string &vertpath, const std::string &fragpath);
	static Mesh* addMesh(const std::string &filepath);
	static Material* addMaterial(const std::string &filepath);
	static Texture* addTexture(const std::string &filepath, bool mipmap = true, bool sRGB = false);

	static std::vector<ShaderProgram*> Shaders;
	static std::vector<Transform*> Transforms;
	static std::vector<Camera*> Cameras;

	static std::unordered_map<std::string, Mesh*> Meshes;
	static std::unordered_map<std::string, Material*> Materials;
	static std::unordered_map<std::string, ShaderProgram*> ShadersList;
	static std::unordered_map<std::string, Texture*> Textures;
};

typedef ResourceManager rm;