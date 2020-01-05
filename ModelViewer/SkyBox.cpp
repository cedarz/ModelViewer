#include "SkyBox.h"
#include "ForShader.h"
#include "Game.h"
#include "InputHandler.h"

#include <FreeImagePlus.h>
#include <iostream>
#include <vector>
#include <cstring>


SkyBox::SkyBox()
{

}


SkyBox::~SkyBox()
{
	glDeleteTextures(1, &cube_texture_id);
}

void SkyBox::init(char* folder)
{
	float offset_side_cube = 1.0f;

	GLfloat skybox_vertices[] = {
		// Positions          
		-offset_side_cube,  offset_side_cube, -offset_side_cube, // front
		-offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,

		-offset_side_cube, -offset_side_cube,  offset_side_cube, // left
		-offset_side_cube, -offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,

		offset_side_cube, -offset_side_cube, -offset_side_cube, // right
		offset_side_cube, -offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube, -offset_side_cube,
		offset_side_cube, -offset_side_cube, -offset_side_cube,

		-offset_side_cube, -offset_side_cube,  offset_side_cube, // back
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,

		-offset_side_cube,  offset_side_cube, -offset_side_cube, // top
		 offset_side_cube,  offset_side_cube, -offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube,  offset_side_cube,  -offset_side_cube,

		-offset_side_cube, -offset_side_cube, -offset_side_cube, // bottom
		 offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube, -offset_side_cube,  offset_side_cube,
		 offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube, -offset_side_cube
	};

	// VBO
	glGenBuffers(1, &VBO_vertices_textures);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices_textures);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// VAO
	glGenVertexArrays(1, &VAO_skybox);
	glBindVertexArray(VAO_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices_textures);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cube_texture_id = createCubeTexture(folder);

	skybox_shaders = ForShader::makeProgram("shaders/skybox.vert", "shaders/skybox.frag");

}

void SkyBox::update(glm::mat4 VP_matr)
{
	// another skybox

	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_5))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("skybox"); //("images/skybox_morning");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_6))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("skybox"); //("images/skybox_ayden");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_7))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("skybox"); //("images/skybox_drakeq");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_8))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("skybox"); //("images/skybox_mercury");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_9))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("skybox"); //("images/skybox_shadow");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_0))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("skybox"); //("images/skybox_violentday");
	}

	// get MV matrix from world around whom draw skybox
	VP_matrix = VP_matr;
}

void SkyBox::draw() {
	// draw skybox LAST in scene ( optimization in vertex shader )
	glDepthFunc(GL_LEQUAL); // optimization in shaders
	glUseProgram(skybox_shaders);
	glBindVertexArray(VAO_skybox);
	glUniformMatrix4fv(glGetUniformLocation(skybox_shaders, "VP"), 1, GL_FALSE, glm::value_ptr(VP_matrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS); // return to default
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
GLuint SkyBox::createCubeTexture(char* folder)
{
	GLuint text_id;
	glGenTextures(1, &text_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, text_id);

	std::vector<std::string> image_path; // same name single textures must be in folder
	image_path.push_back(std::string(folder) + std::string("/right.jpg"));
	image_path.push_back(std::string(folder) + std::string("/left.jpg"));
	image_path.push_back(std::string(folder) + std::string("/top.jpg"));
	image_path.push_back(std::string(folder) + std::string("/bottom.jpg"));
	image_path.push_back(std::string(folder) + std::string("/front.jpg"));
	image_path.push_back(std::string(folder) + std::string("/back.jpg"));


	int width = 0;
	int height = 0;
	int bpp = 0;
	
	for (int i = 0; i < 6; i++) {
		fipImage face;
		face.load(image_path[i].c_str());
		face.flipVertical();
		width = face.getWidth();
		height = face.getHeight();
		bpp = face.getBitsPerPixel();

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, face.accessPixels());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return text_id;
}

