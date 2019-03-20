#include "GameObject.h"
#include "ResourceManager.h"

GameObject::GameObject()
{
}

GameObject::GameObject(const std::string & meshFile, const std::vector<std::string>& textureFiles)
{
	setMesh(rm::addMesh(meshFile));
	bool sRGB = true; // First texture is our albedo, we'll want that to be sRGB, nothing else needs to be 
	for (std::string file : textureFiles)
	{
		textures.push_back(rm::addTexture(file, true, sRGB));
		sRGB = false;
	}
}

GameObject::GameObject(Mesh * _mesh, Texture * _texture)
{
	setMesh(_mesh);
	setTexture(_texture);
}

GameObject::GameObject(Mesh * _mesh, std::vector<Texture*> &_textures)
{
	setMesh(_mesh);
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
	textures.clear();
	textures.push_back(_texture);
}

void GameObject::setTextures(std::vector<Texture*> &_textures)
{
	textures.clear();
	for (Texture* texture : _textures)
	{
		textures.push_back(texture);
	}
}

void GameObject::setShaderProgram(ShaderProgram * _shaderProgram)
{
	material = _shaderProgram;
}

void GameObject::draw()
{
	material->bind();
	material->sendUniform("uModel", getLocalToWorld());
	int i = 0;
	for (Texture* texture : textures)
	{
		texture->bind(i++);
	}
	mesh->draw();
	for (Texture* texture : textures)
	{
		texture->unbind(--i);
	}
}
