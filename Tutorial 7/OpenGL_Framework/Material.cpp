#define _CRT_SECURE_NO_WARNINGS 
#include "Material.h"
#include "ResourceManager.h"

#include <fstream>

Texture* Material::blackTex;
Texture* Material::whiteTex;
Texture* Material::normalTex;

Material::Material(const std::string & filename)
{
	std::string file = "../assets/materials/" + filename;
	std::ifstream inFile(file);
	char lineChar[256];
	char line[256];
	vec4ui u;
	vec4i i;
	vec4 f;
	unsigned int mipmaps;
	unsigned int sRGB;
	unsigned int bindSlot;
	int lineNumber = 0;
	if(inFile.fail())
	{ 
		SAT_DEBUG_LOG_ERROR("Could not read material file \n%s", filename.c_str());
		system("PAUSE");
		return;
	}

	while (!inFile.eof())
	{
		inFile.getline(line, 256);
		++lineNumber;

		if (line[0] == ';' || line[0] == '#' || (line[0] == '/' && line[1] == '/'))
		{
			// Comment
		}
		else if (!strncmp(line, "PBR", 3))
		{
			textures.resize(5);
			texturesBindLocation.resize(5);
			textures[0] = whiteTex;
			textures[1] = blackTex;
			textures[2] = whiteTex;
			textures[3] = whiteTex;
			textures[4] = normalTex;
			for (unsigned int k = 0; k < 5; ++k)
				texturesBindLocation[k] = k;

			floatUniforms["uRoughness"] = 1.0f;
			floatUniforms["uMetallic"] = 0.0f;

			vertShader = "shader.vert"; 
			fragShader = "gBuffer.frag";
		}
		else if (std::sscanf(line, "texture: %u %u %u %s", &bindSlot, &mipmaps, &sRGB, lineChar) == 4)
		{
			addTexture(rm::addTexture(lineChar, mipmaps, sRGB), bindSlot);
		}
		else if (std::sscanf(line, "texture: %u %u %s", &mipmaps, &sRGB, lineChar) == 3)
		{
			addTexture(rm::addTexture(lineChar, mipmaps, sRGB), textures.size());
		}
		else if (std::sscanf(line, "texture: %s", lineChar) == 1)
		{
			addTexture(rm::addTexture(lineChar), textures.size());
		}
		// Special Textures
		else if (std::sscanf(line, "textureAlbedo: %s", lineChar) == 1)
		{
			addTexture(rm::addTexture(lineChar, true, true), pbrAlbedo);
		}
		else if (std::sscanf(line, "textureEmissive: %s", lineChar) == 1)
		{
			addTexture(rm::addTexture(lineChar, true, true), pbrEmissive);
		}
		else if (std::sscanf(line, "textureRoughness: %s", lineChar) == 1)
		{
			addTexture(rm::addTexture(lineChar), pbrRoughness);
		}
		else if (std::sscanf(line, "textureMetallic: %s", lineChar) == 1)
		{
			addTexture(rm::addTexture(lineChar), pbrMetallic);
		}
		else if (std::sscanf(line, "textureNormal: %s", lineChar) == 1)
		{
			addTexture(rm::addTexture(lineChar), pbrNormal);
		}
		// Uniforms
		else if (std::sscanf(line, "vec4: %s %f %f %f %f", lineChar, &f.x, &f.y, &f.z, &f.w) == 5)
		{
			vec4Uniforms[lineChar] = vec4(f.x, f.y, f.z, f.w);
		}
		else if (std::sscanf(line, "float: %s %f", lineChar, &f.x) == 2)
		{
			floatUniforms[lineChar] = f.x;
		}
		else if (std::sscanf(line, "int: %s %d", lineChar, &i.x) == 2)
		{
			intUniforms[lineChar] = i.x;
		}
		// Shaders
		else if (std::sscanf(line, "vertShader: %s", lineChar) == 1)
		{
			vertShader = lineChar;
		}
		else if (std::sscanf(line, "fragShader: %s", lineChar) == 1)
		{
			fragShader = lineChar;
		}
		else
		{
			SAT_DEBUG_LOG_WARNING("Could not parse line in material file \n%s\n%i: %s", filename.c_str(), lineNumber, line);
		}
	}

	SAT_ASSERT(vertShader != "" && fragShader != "", "Shaders not set!");
	shader = rm::addShader(vertShader, fragShader);
}

void Material::initDefaultTextures()
{
	blackTex  = rm::addTexture("default/defaultBlack.png");
	whiteTex  = rm::addTexture("default/defaultWhite.png");
	normalTex = rm::addTexture("default/defaultNormal.png");
}

void Material::addTexture(Texture * texture, unsigned int slot)
{
	unsigned int index = 0;
	if (checkDuplicate(index, slot))
	{
		textures[index] = texture;
		texturesBindLocation[index] = slot;
	}
	else
	{
		textures.push_back(texture);
		texturesBindLocation.push_back(slot);
	}
}

bool Material::checkDuplicate(unsigned int &index, unsigned int bindSlot)
{
	for (unsigned int i = 0; i < texturesBindLocation.size(); ++i)
	{
		if (texturesBindLocation[i] == bindSlot)
		{
			index = i;
			return true;
		}
	}
	return false;
}
