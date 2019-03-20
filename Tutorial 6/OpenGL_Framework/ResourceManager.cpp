#include "ResourceManager.h"
#include "Mesh.h"
#include "Texture.h"

std::vector<Transform*> ResourceManager::Transforms;
std::vector<ShaderProgram*> ResourceManager::Shaders;
std::vector<Camera*> ResourceManager::Cameras;
std::unordered_map<std::string, Mesh*> ResourceManager::Meshes;
std::unordered_map<std::string, Texture*> ResourceManager::Textures;

void ResourceManager::addEntity(Transform * entity)
{
	Transforms.push_back(entity);
	if (Camera* camera = dynamic_cast<Camera*>(entity))
	{
		Cameras.push_back(camera);
	}
	else
	{

	}
}

void ResourceManager::addShader(ShaderProgram * shader)
{
	Shaders.push_back(shader);
}

Mesh* ResourceManager::addMesh(const std::string &filepath)
{
	if (Meshes.find(filepath) == Meshes.end())
	{
		Mesh* newMesh = new Mesh();
		newMesh->loadFromObj(filepath);
		Meshes[filepath] = newMesh;
		return newMesh;
	}
	return Meshes.at(filepath);
}

Texture * ResourceManager::addTexture(const std::string & filepath, bool mipmap, bool sRGB)
{
	if (Textures.find(filepath) == Textures.end())
	{
		Texture* newTexture = new Texture();
		newTexture->load(filepath, mipmap, sRGB);
		Textures[filepath] = newTexture;
		return newTexture;
	}
	return Textures.at(filepath);
}
