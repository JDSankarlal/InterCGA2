#pragma once
#include "GL/glew.h"
#include "Transform.h"
#include <vector>
#include "VertexBufferObject.h"

class Mesh
{
public:
	void initWireframeCube();
	void initMeshSphere(const unsigned int xSlices, const unsigned int ySlices, bool invert = false);
	void initMeshPlane(const unsigned int xDivisions, const unsigned int yDivisions);
	
	bool loadFromObj(const std::string &file);

	std::vector<vec4> dataVertex;
	std::vector<vec4> dataTexture;
	std::vector<vec4> dataNormal;
	std::vector<vec4> dataTangent;
	std::vector<vec4> dataBiTangent;
	std::vector<vec4> dataColor;

	void draw() const;
	void calculateTangents();
	void Mesh::bind() const;
	static void Mesh::unbind();
private:
	VertexArrayObject vao;
	bool _IsLoaded = false;
	static std::string _ModelDirectory;

	void uploadToGPU();
};