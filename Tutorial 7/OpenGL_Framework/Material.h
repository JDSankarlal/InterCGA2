#pragma once
#include "ShaderProgram.h"
#include "Texture.h"
#include <unordered_map>

enum TextureMaterialPBR
{
	pbrAlbedo = 0,
	pbrEmissive = 1,
	pbrRoughness = 2,
	pbrMetallic = 3,
	pbrNormal = 4,
};

class Material
{
public:
	ShaderProgram* shader = nullptr;

	std::unordered_map<std::string, vec4> vec4Uniforms;
	std::unordered_map<std::string, mat4> mat4Uniforms;
	std::unordered_map<std::string, int> intUniforms;
	std::unordered_map<std::string, float> floatUniforms;
	// More types???

	std::vector<Texture*> textures;
	std::vector<GLuint> texturesBindLocation;

	std::string name;
	std::string vertShader = "";
	std::string fragShader = "";

	Material()
	{

	}

	Material(const std::string &filename);

	void sendUniforms()
	{
		// Send vector4 uniforms
		for (auto itr = vec4Uniforms.begin(); itr != vec4Uniforms.end(); itr++)
			shader->sendUniform(itr->first, itr->second);

		// Send mat4 uniforms
		for (auto itr = mat4Uniforms.begin(); itr != mat4Uniforms.end(); itr++)
			shader->sendUniform(itr->first, itr->second);

		// Send int uniforms
		for (auto itr = intUniforms.begin(); itr != intUniforms.end(); itr++)
			shader->sendUniform(itr->first, itr->second);

		// Send float uniforms
		for (auto itr = floatUniforms.begin(); itr != floatUniforms.end(); itr++)
			shader->sendUniform(itr->first, itr->second);
	}

	void bind()
	{
		shader->bind();
	}

	void unbind()
	{
		shader->unbind();
	}

	static void initDefaultTextures();
	static Texture* blackTex;
	static Texture* whiteTex;
	static Texture* normalTex;

	void addTexture(Texture* texture, unsigned int slot);
private:
	bool checkDuplicate(unsigned int &index, unsigned int bindSlot);
};