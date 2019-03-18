#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include <vector>

/*
  ////////////////
 // GameObject //
////////////////

The GameObject class is used for easily representing objects in the scene, 
each object gets a reference to a mesh, multiple textures, and a material,
which determines what shader the object is drawn with

*/

class GameObject : public Transform
{
public:
	GameObject();
	GameObject(const std::string &meshFile, const std::string &materialFile);
	GameObject(const std::string &meshFile, const std::vector <std::string> &textureFiles); 
	GameObject(Mesh* _mesh, Texture *_texture);
	GameObject(Mesh* _mesh, const std::string &materialFile);
	GameObject(Mesh* _mesh, std::vector <Texture*>& _textures);
	~GameObject();

	void setMesh(Mesh* _mesh);
	void setTexture(Texture* _texture);
	void setTextures(std::vector <Texture*>& _textures);
	void setShaderProgram(ShaderProgram* _shaderProgram);
	void draw();

	Material* material = nullptr;
private:
	Mesh* mesh;
};