#pragma once
#include <string>
#include <stdio.h>
#define NOMINMAX
#include <Windows.h>
#include "GL/glew.h"

/*
Some handy input/output functions I made
SAT_ASSERT(expression, message, ...) will only run when debug logging is active, 
	and will print out the location it was called along with the expression that failed
*/

std::string readFile(const std::string &filename);

std::string zeroPadNumber(unsigned int num, unsigned int padding);

void setOpenGLName(GLenum identifier, GLuint name, const std::string &label);

#define SAT_LOG_ERROR(message, ...)			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);\
											printf(message, ##__VA_ARGS__);	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");


#define __SAT_STRINGIFY(x) #x
#define __SAT_TOSTRING(x) __SAT_STRINGIFY(x)
#define __SAT_AT "[" __FILE__ ":" __SAT_TOSTRING(__LINE__) "]"
#define __SAT_FUNCTION __FUNCSIG__ 
#define SAT_OP_MOD %


#define SAT_FUNCTION_NAME()					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C); printf("%s", __SAT_FUNCTION); \
											SAT_LOG_ERROR("")

#define SAT_ERROR_LOC(message, ...)			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C); printf("%s\n", __SAT_AT); \
											SAT_LOG_ERROR(message, ##__VA_ARGS__)

#if _DEBUG || DEBUG_LOG

#define SAT_DEBUG_LOG(message, ...)			printf(message, ##__VA_ARGS__); printf("\n");

#define SAT_DEBUG_LOG_ERROR(message, ...)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);\
											printf(message, ##__VA_ARGS__);	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");

#define SAT_DEBUG_LOG_WARNING(message, ...)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0E); printf(message, ##__VA_ARGS__);\
											SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");

#define SAT_DEBUG_LOG_SAFE(message, ...)	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0A); printf(message, ##__VA_ARGS__);\
											SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07); printf("\n");

#define SAT_ASSERT(expression, message, ...)	if(!(expression)) { SAT_DEBUG_LOG_WARNING(#expression); SAT_FUNCTION_NAME(); SAT_ERROR_LOC(message, ##__VA_ARGS__); system("PAUSE");}
#define SAT_ASSERT_NOPAUSE(expression, message, ...)	if(!(expression)) { SAT_DEBUG_LOG_WARNING(#expression); SAT_FUNCTION_NAME(); SAT_ERROR_LOC(message, ##__VA_ARGS__); }

#define SAT_ASSERT_ALIGN(expression, type)	SAT_ASSERT(#expression, "\n[UniformBuffer %d of size %d] %s is not aligned to GPU!", _Handle, _BytesAllocated, type)



#else

#define SAT_DEBUG_LOG			__noop
#define SAT_DEBUG_LOG_ERROR		__noop	
#define SAT_DEBUG_LOG_WARNING	__noop
#define SAT_DEBUG_LOG_SAFE		__noop
#define SAT_ASSERT				__noop
#define SAT_ASSERT_NOPAUSE		__noop
#define SAT_ASSERT_ALIGN		__noop

#endif