#include "ShaderProgram.h"
#include "IO.h"
#include <fstream>
#include "Camera.h"

bool ShaderProgram::_IsInitDefault = false;

GLuint ShaderProgram::_ProgramHandleBinded = GL_NONE;

std::string ShaderProgram::_ShaderDirectory = "../assets/shaders/";
GLuint ShaderProgram::_VertShaderDefault = 0;
GLuint ShaderProgram::_FragShaderDefault = 0;
GLuint ShaderProgram::_ProgramDefault = 0;

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::ShaderProgram(const std::string & vertFile, const std::string & fragFile)
{
	load(vertFile, fragFile);
}

ShaderProgram::~ShaderProgram()
{
	if (_IsInit)
	{
		unload();
	}
}

bool ShaderProgram::initDefault()
{
	if (!_IsInitDefault)
	{
		ShaderProgram errorShader; 
		bool compileSuccess = errorShader.load("error.vert", "error.frag");
		if (!compileSuccess)
		{
			SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Could not compile default Shaders!");
			system("PAUSE");
			exit(EXIT_FAILURE);
		}
		ShaderProgram::_VertShaderDefault = errorShader._VertShaderHandle;
		ShaderProgram::_FragShaderDefault   = errorShader._FragShaderHandle;
		ShaderProgram::_ProgramDefault		= errorShader._ProgramHandle;
		_IsInitDefault = true;
		setOpenGLName(GL_SHADER, _VertShaderDefault, "Shader: " + errorShader._VertFilename);
		setOpenGLName(GL_SHADER, _FragShaderDefault, "Shader: " + errorShader._FragFilename);
		setOpenGLName(GL_PROGRAM, _ProgramDefault, "Default ShaderProgram");
	}
	return _IsInitDefault;
}

void ShaderProgram::setDefault()
{
	_VertShaderHandle = _VertShaderDefault;
	_FragShaderHandle = _FragShaderDefault;
	_ProgramHandle = _ProgramDefault;
}

bool ShaderProgram::load(const std::string & vertFile, const std::string & fragFile)
{
	_VertFilename = vertFile;
	_FragFilename = fragFile;
	_ProgramName = _VertFilename + " " + _FragFilename;

	// Create shader and program objects
	_VertShaderHandle = glCreateShader(GL_VERTEX_SHADER);
	_FragShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	setOpenGLName(GL_SHADER, _VertShaderHandle, "VertShader: " + _VertFilename);
	setOpenGLName(GL_SHADER, _FragShaderHandle, "FragShader: " + _FragFilename);
	_ProgramHandle = glCreateProgram();
	setOpenGLName(GL_PROGRAM, _ProgramHandle, "Program: " + _ProgramName);

	// Load our source code for shaders
	std::string source = readFile(_ShaderDirectory + vertFile);
	const GLchar *temp = static_cast<const GLchar *>(source.c_str());
	glShaderSource(_VertShaderHandle, 1, &temp, NULL);

	source = readFile(_ShaderDirectory + fragFile);
	temp = static_cast<const GLchar *>(source.c_str());
	glShaderSource(_FragShaderHandle, 1, &temp, NULL);

	// Compile vertex and frag shaders
	if (!compileShader(_VertShaderHandle))
	{
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Vertex Shader failed to compile.");
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", vertFile.c_str());
		outputShaderLog(_VertShaderHandle);
		unload();
		setDefault();
		return false;
	}

	if (!compileShader(_FragShaderHandle))
	{
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Fragment Shader failed to compile.");
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", fragFile.c_str());
		outputShaderLog(_FragShaderHandle);
		unload();
		setDefault();
		return false;
	}

	glAttachShader(_ProgramHandle, _VertShaderHandle);
	glAttachShader(_ProgramHandle, _FragShaderHandle);

	if (!linkProgram())
	{
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Shader Program failed to link.");
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", vertFile.c_str());
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", fragFile.c_str());
		outputProgramLog();
		unload();
		setDefault();
		return false;
	}

	_IsInit = true;
	return true;
}

bool ShaderProgram::reload()
{
	return load(_VertFilename, _FragFilename);
}

bool ShaderProgram::isLoaded() const
{
	return _IsInit;
}

void ShaderProgram::unload()
{
	// If the Program is deleted, all attached shaders are automatically detached 
	// This means it is not neccesary to detach them manually in this case
	if (_ProgramHandle != 0 && _ProgramHandle != _ProgramDefault) 
	{
		glDeleteProgram(_ProgramHandle);
		_ProgramHandle = 0;
	}
	if (_VertShaderHandle != 0 && _VertShaderHandle != _VertShaderDefault) //If the Vertex shader exists, remove it.
	{
		//glDetachShader(_Program, _VertShader);
		glDeleteShader(_VertShaderHandle);
		_VertShaderHandle = 0;
	}
	if (_FragShaderHandle != 0 && _FragShaderHandle != _FragShaderDefault) //If the Fragment shader exists, remove it.
	{
		//glDetachShader(_Program, _FragShader);
		glDeleteShader(_FragShaderHandle);
		_FragShaderHandle = 0;
	}
	
	_IsInit = false;
}

bool ShaderProgram::linkProgram()
{
	glLinkProgram(_ProgramHandle);
	GLint success;
	glGetProgramiv(_ProgramHandle, GL_LINK_STATUS, &success);
	return success == GL_TRUE;
}

void ShaderProgram::bind() const
{
	_ProgramHandleBinded = _ProgramHandle;
	glUseProgram(_ProgramHandle);
}

void ShaderProgram::unbind() const
{
	_ProgramHandleBinded = GL_NONE;
	glUseProgram(GL_NONE);
}

void ShaderProgram::bindUBO(const std::string & uniformBlockName, unsigned int bindSlot) const
{
	GLuint index = glGetUniformBlockIndex(_ProgramHandle, uniformBlockName.c_str());
	glUniformBlockBinding(_ProgramHandle, index, bindSlot);
}

GLint ShaderProgram::getUniformLocation(const std::string & uniformName) const
{
	GLint uniformLoc = glGetUniformLocation(_ProgramHandle, uniformName.c_str());	
#if _DEBUG || DEBUG_LOG
	if (_ProgramHandle != _ProgramHandleBinded)
	{
		SAT_DEBUG_LOG_WARNING("[ShaderProgram.cpp] \"%s\" is not currently binded to recieve %s!", _ProgramName.c_str(), uniformName.c_str());
	}
	if (uniformLoc == -1)
	{
		SAT_DEBUG_LOG_WARNING("[ShaderProgram.cpp] Uniform %s not found in %s!", uniformName.c_str(), _ProgramName.c_str());
	}
#endif
	return uniformLoc;
}

void ShaderProgram::sendUniform(const std::string & name, const float scalar) const
{
	GLint location = getUniformLocation(name);
	glUniform1f(location, scalar);
}

void ShaderProgram::sendUniform(const std::string & name, const int integer) const
{
	GLint location = getUniformLocation(name);
	glUniform1i(location, integer);
}

void ShaderProgram::sendUniform(const std::string & name, const vec2 & vector) const
{
	GLint location = getUniformLocation(name);
	glUniform2fv(location, 1, &vector.x);
}

void ShaderProgram::sendUniform(const std::string & name, const vec3 & vector) const
{
	GLint location = getUniformLocation(name);
	glUniform3fv(location, 1, &vector.x);
}

void ShaderProgram::sendUniform(const std::string & name, const vec4 & vector) const
{
	GLint location = getUniformLocation(name);
	glUniform4fv(location, 1, &vector.x);
}

void ShaderProgram::sendUniform(const std::string & name, const mat4 & matrix, bool transpose) const
{
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, transpose, matrix.arr);
}

void ShaderProgram::sendUniformCamera(Camera * camera)
{
	sendUniform("uView", camera->getView());
	sendUniform("uProj", camera->getProjection());
}

bool ShaderProgram::isShaderBinded()
{
	SAT_ASSERT(_ProgramHandle == _ProgramHandleBinded, "Cannot send uniform to a ShaderProgram that isn't bound!");
	return _ProgramHandle == _ProgramHandleBinded;
}

bool ShaderProgram::compileShader(GLuint shader) const
{
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	return success == GL_TRUE;
}

void ShaderProgram::outputShaderLog(GLuint shader) const
{
	std::vector<char> infoLog;
	infoLog.resize(2048);

	GLint infoLen;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	glGetShaderInfoLog(shader, sizeof(char) * 2048, &infoLen, &infoLog[0]); //Size of char array in bits for portability, rather than characters.
	SAT_DEBUG_LOG_WARNING("%s", std::string(infoLog.begin(), infoLog.end()).c_str());
}

void ShaderProgram::outputProgramLog() const
{
	std::vector<char> infoLog;
	infoLog.resize(2048);

	GLint infoLen;
	glGetProgramiv(_ProgramHandle, GL_INFO_LOG_LENGTH, &infoLen);
	glGetProgramInfoLog(_ProgramHandle, sizeof(char) * 2048, &infoLen, &infoLog[0]); // Size of char array in bits for portability, rather than characters.
	SAT_DEBUG_LOG_WARNING("%s", std::string(infoLog.begin(), infoLog.end()).c_str());
}
