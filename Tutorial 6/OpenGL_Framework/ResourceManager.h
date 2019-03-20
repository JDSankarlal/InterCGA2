#pragma once
#include <unordered_map>
#include "GameObject.h"
#include "Camera.h"

class ResourceManager
{
public:
	static void addEntity(Transform* entity);
	static void addShader(ShaderProgram* shader);
	static Mesh* addMesh(const std::string &filepath);
	static Texture* addTexture(const std::string &filepath, bool mipmap = true, bool sRGB = false);

	static std::vector<ShaderProgram*> Shaders;
	static std::vector<Transform*> Transforms;
	static std::vector<Camera*> Cameras;

	static std::unordered_map<std::string, Mesh*> Meshes;
	static std::unordered_map<std::string, Texture*> Textures;
};

typedef ResourceManager rm;