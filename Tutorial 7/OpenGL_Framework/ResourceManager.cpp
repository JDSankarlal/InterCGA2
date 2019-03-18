#include "ResourceManager.h"
#include "Mesh.h"
#include "Texture.h"

std::vector<Transform*> ResourceManager::Transforms;
std::vector<ShaderProgram*> ResourceManager::Shaders;
std::vector<Camera*> ResourceManager::Cameras;

std::unordered_map<std::string, Mesh*> ResourceManager::Meshes;
std::unordered_map<std::string, Material*> ResourceManager::Materials;
std::unordered_map<std::string, ShaderProgram*> ResourceManager::ShadersList;
std::unordered_map<std::string, Texture*> ResourceManager::Textures;

Transform* ResourceManager::addEntity(Transform * entity)
{
	Transforms.push_back(entity);
	if (Camera* camera = dynamic_cast<Camera*>(entity))
	{
		Cameras.push_back(camera);
	}
	else
	{

	}
	return entity;
}

void ResourceManager::addShader(ShaderProgram * shader)
{
	Shaders.push_back(shader);
}

ShaderProgram* ResourceManager::addShader(const std::string & vertpath, const std::string & fragpath)
{
	if (ShadersList.find(vertpath + " " + fragpath) == ShadersList.end())
	{
		ShaderProgram* newShader = new ShaderProgram();
		newShader->load(vertpath, fragpath);
		ShadersList[vertpath + " " + fragpath] = newShader;
		return newShader;
	}
	return ShadersList.at(vertpath + " " + fragpath);
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

Material * ResourceManager::addMaterial(const std::string & filepath)
{
	if (Materials.find(filepath) == Materials.end())
	{
		Material* newMaterial = new Material(filepath);
		Materials[filepath] = newMaterial;
		return newMaterial;
	}
	return Materials.at(filepath);
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
