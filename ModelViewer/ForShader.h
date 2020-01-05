#ifndef FOR_SHADER_H
#define FOR_SHADER_H
#include <glad/glad.h>
#include "SDL/SDL_opengl.h"

#include <iostream>

class ForShader
{
public:
	ForShader();
	~ForShader();

	static const GLchar* readShader(const char* filename);

	static GLuint makeProgram(const char* vertex, const char* fragment);
};
#endif // !FOR_SHADER_H

