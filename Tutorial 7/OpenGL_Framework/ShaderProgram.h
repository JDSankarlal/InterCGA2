#pragma once
#include <string>
#include <gmtk/gmtk.h>
#include "GL\glew.h"
#include <vector>

/*
  ///////////////////
 // ShaderProgram //
///////////////////

Shader compilation is achieved in OpenGL through Program Objects, which contain 
the code for all of the Shader stages, so that everything needed to render is 
attached to a single Program Object. Shaders with multiple stages require a 
two-stage compilation process, similar to C++ itself. C++ source code is fed into 
a compiler producing individual object files, which are then linked together to 
get the executable code, Shader source code is also compiled into "Shader Objects", 
which are then linked together to get the executable Program Object.

(GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER)
Shader Objects are assigned a type when generated, and given a text string 
representing the GLSL code. Since the code is sent to the Shader Object, this 
actually means that the compiling is done on your GPU. Once the Shader Objects 
have been compiled, it is possible to attach and link them to the Program Object.  
Keep in mind that Vertex input and Fragment output locations cannot be modified after linking!

Program Objects can send back the binary data of the compiled shader meaning it is not 
neccesary to recompile the source code when there are no changes. Loading in the binary 
data on subsequent runs can decrease load times. Program Binary are not transferrable,
different hardware vendors may not accept the same binary code, different hardware from
the same vendor may still not accept the same code. Even the SAME MACHINE may not accept 
the code due to driver updates. glProgramBinary can fail frequently, so it is important to 
have a fallback for creating the shaders.
More info about binary loading can be read here
https://www.khronos.org/opengl/wiki/Shader_Compilation#Binary_upload

It is also possible to compile Shaders using SPIR-V!

A good Shader Program will also output an error log when an error is encountered.

More info about Shader Compilation can be read here
https://www.khronos.org/opengl/wiki/Shader_Compilation

Limitations of Program Objects can be found using glGetIntegerv(GL_MACRO, &integer)
	-POs have a maximum number of uniform locations
		GL_MAX_UNIFORM_LOCATIONS
	-There is a maximum string length of a uniform name
		GL_ACTIVE_UNIFORM_MAX_LENGTH

Subroutines can be useful for creating variations of code inside the same shader

More info about Shader Subroutines can be read here
https://www.khronos.org/opengl/wiki/Shader_Subroutine

Limitations of Subroutines can be found using glGetIntegerv(GL_MACRO, &integer)
	-There is a maximum number of subroutine functions (at least 256)
		GL_MAX_SUBROUTINES
	-There is a maximum number of individual subroutine locations (at least 1024)
		GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS

*/



class Camera;
class Transform;

class ShaderProgram
{
public: 
	ShaderProgram();
	ShaderProgram(const std::string &vertFile, const std::string &fragFile);
	~ShaderProgram();

	static bool initDefault();
	void setDefault();

	bool load(const std::string &vertFile, const std::string &fragFile);
	bool reload();
	bool isLoaded() const;
	void unload();
	bool linkProgram();

	void bind() const;
	void unbind() const;
	void bindUBO(const std::string & uniformBlockName, unsigned int bindSlot) const;

	GLint getUniformLocation(const std::string &uniformName) const;

	void sendUniform(const std::string &name, const float scalar) const;
	void sendUniform(const std::string &name, const int integer) const;
	void sendUniform(const std::string &name, const vec2 &vector) const;
	void sendUniform(const std::string &name, const vec3 &vector) const;
	void sendUniform(const std::string &name, const vec4 &vector) const;
	void sendUniform(const std::string &name, const mat4 &matrix, bool transpose = false) const;
	void sendUniformCamera(Camera *camera);

private: 
	bool isShaderBinded();

	bool _IsInit = false;
	GLuint _VertShaderHandle = 0;
	GLuint _FragShaderHandle = 0;
	GLuint _ProgramHandle = 0;


	std::string _VertFilename;
	std::string _FragFilename;
	std::string _ProgramName;

	static std::string _ShaderDirectory;

	static GLuint _ProgramHandleBinded;
	static bool _IsInitDefault;
	static GLuint _VertShaderDefault;
	static GLuint _FragShaderDefault;
	static GLuint _ProgramDefault;

	bool compileShader(GLuint shader) const;
	void outputShaderLog(GLuint shader) const;
	void outputProgramLog() const;
};