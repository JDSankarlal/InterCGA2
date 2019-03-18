#include "GameObject.h"
#include "ResourceManager.h"

GameObject::GameObject()
{
}

GameObject::GameObject(const std::string & meshFile, const std::string & materialFile)
{
	setMesh(rm::addMesh(meshFile));
	material = rm::addMaterial(materialFile);
}

GameObject::GameObject(const std::string & meshFile, const std::vector<std::string>& textureFiles)
{
	setMesh(rm::addMesh(meshFile));
	bool sRGB = true; // First texture is our albedo, we'll want that to be sRGB, nothing else needs to be 
	if (material == nullptr)
	{
		material = new Material();
	}
	for (std::string file : textureFiles)
	{
		material->textures.push_back(rm::addTexture(file, true, sRGB));
		sRGB = false;
	}
}

GameObject::GameObject(Mesh * _mesh, Texture * _texture)
{
	setMesh(_mesh);
	if (material == nullptr)
	{
		material = new Material();
	}
	setTexture(_texture);
}

GameObject::GameObject(Mesh* _mesh, const std::string &materialFile)
{
	setMesh(_mesh);
	material = rm::addMaterial(materialFile);
}

GameObject::GameObject(Mesh * _mesh, std::vector<Texture*> &_textures)
{
	setMesh(_mesh);
	if (material == nullptr)
	{
		material = new Material();
	}
	setTextures(_textures);
}

GameObject::~GameObject()
{

}

void GameObject::setMesh(Mesh * _mesh)
{
	mesh = _mesh;
}

void GameObject::setTexture(Texture * _texture)
{
	material->textures.clear();
	material->textures.push_back(_texture);
}

void GameObject::setTextures(std::vector<Texture*> &_textures)
{
	material->textures.clear();
	for (Texture* texture : _textures)
	{
		material->textures.push_back(texture);
	}
}

void GameObject::setShaderProgram(ShaderProgram * _shaderProgram)
{
	if (material == nullptr)
	{
		material = new Material();
	}
	material->shader = _shaderProgram;
}

void GameObject::draw()
{
	SAT_ASSERT(material != nullptr, "GameObject %s has no material!", name.c_str());
	material->bind();
	material->sendUniforms();
	material->shader->sendUniform("uModel", getLocalToWorld());
	int i = 0;
	for (Texture* texture : material->textures)
	{
		texture->bind(i++);
	}
	mesh->draw();
	for (Texture* texture : material->textures)
	{
		texture->unbind(--i);
	}
}
