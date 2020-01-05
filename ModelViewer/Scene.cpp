#include "Scene.h"
#include "Game.h"
#include "SkyBox.h"
#include "TextRendering.h"
#include "ForShader.h"
#include "FreeImagePlus.h"

Scene::Scene()
{
	std::cout << "triangle construktor()" << std::endl;

	glm::vec3 position(2.0f, 5.0f, 18.0f);
	camera = Camera(position);

	mouse_first_in = true;
}


Scene::~Scene()
{
	glDeleteProgram(shaders_animated_model);
	Mix_FreeMusic(music1);
}


void Scene::init() {
	// shader for animated model
	shaders_animated_model = ForShader::makeProgram("shaders/animated_model.vert", "shaders/animated_model.frag");

	model_man.loadModel("models/man/model.dae");
	model_man.initShaders(shaders_animated_model);

	//matr_model = glm::scale(matr_model, glm::vec3(0.1f, 0.1f, 0.1f));
	matr_model_1 = glm::rotate(matr_model_1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	model_astroboy.loadModel("models/astroboy/astroBoy_walk_Max.dae");
	model_astroboy.initShaders(shaders_animated_model);
	matr_model_2 = glm::rotate(matr_model_2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//matr_model2 = glm::scale(matr_model, glm::vec3(0.3f, 0.3f, 0.3f));
	matr_model_2 = glm::translate(matr_model_2, glm::vec3(5.0f, 0.0f, 0.0f));

	// skybox
	SkyBox::Instance()->init("skybox");

	// text 2D
	text_matrix_2D = glm::ortho(0.0f, (float)Game::Instance()->screen_width, 0.0f, (float)Game::Instance()->screen_height, 1.0f, -1.0f);

	// music
	music1 = Mix_LoadMUS("music/kraski.mp3");
	Mix_VolumeMusic(16);
	Mix_PlayMusic(music1, -1); // -1 = NONSTOP playing  0 = 1 time 1 = 2  2 = 3

	//music2 = Mix_LoadMUS("music/02_modern_talking_you_can_win_if_you_want.mp3");
}

void Scene::update()
{
	GLfloat current_frame = SDL_GetTicks();
	delta_time = (current_frame - last_frame);
	last_frame = current_frame;

	// camera
	camera.updateKey(delta_time, speed);
	// mouse
	if (InputHandler::Instance()->getMouseButtonState(LEFT_PRESSED) )
	{
		SDL_ShowCursor(SDL_DISABLE);
		mouse_position = InputHandler::Instance()->getMousePosition();

		if (mouse_first_in) // need run ONLY if mouse on window !!!
		{
			last_x = mouse_position.getX();
			last_y = mouse_position.getY();
			mouse_first_in = false;
		}
		//std::cout << mouse_position.getX()  <<  "		"  << mouse_position.getY() << std::endl;
		GLfloat x_offset = mouse_position.getX() - last_x;
		GLfloat y_offset = mouse_position.getY() - last_y;
		last_x = mouse_position.getX();
		last_y = mouse_position.getY();

		camera.updateMouse(x_offset, y_offset);
	}
	if (InputHandler::Instance()->getMouseButtonState(LEFT_RELEASED))
	{
		SDL_ShowCursor(SDL_ENABLE);
		mouse_first_in = true;
	}

	perspective_view = camera.getViewMatrix();
	perspective_projection = glm::perspective(glm::radians(camera.fov), (float)Game::Instance()->screen_width / (float)Game::Instance()->screen_height, 1.0f, 2000.0f); // ïèðàìèäà

	model_man.update();
	//our_model2.update();

	// model 1 matrix (MAN)
	matr_model_1 = glm::translate(matr_model_1, glm::vec3(0.0f, -0.05f, 0.0f));
	//matr_model = glm::scale(matr_model, glm::vec3(0.1f, 0.1f, 0.1f));
	matr_model_1 = glm::rotate(matr_model_1, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f));

	// text 3D
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(0.02f, 0.02f, 0.0f));
	glm::mat4 set_text_to_origin = glm::translate(glm::mat4(), glm::vec3(-1.8f, -0.4f, 0.0f));
	glm::mat4 text_rotate_y = glm::rotate(glm::mat4(), glm::radians(-camera.yaw - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 text_rotate_x = glm::rotate(glm::mat4(), glm::radians(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	
	glm::mat4 text_translate_to_model_1 = glm::translate(glm::mat4(), glm::vec3(matr_model_1[3].x, matr_model_1[3].y + 10.0f, matr_model_1[3].z));
	text_matrix_3D_model_1 = perspective_projection * perspective_view * text_translate_to_model_1 * text_rotate_y * text_rotate_x * set_text_to_origin * scale;

	glm::mat4 text_translate_to_model_2 = glm::translate(glm::mat4(), glm::vec3(matr_model_2[3].x, matr_model_2[3].y + 7.0f, matr_model_2[3].z));
	text_matrix_3D_model_2 = perspective_projection * perspective_view * text_translate_to_model_2 * text_rotate_y * text_rotate_x * set_text_to_origin * scale;

	// delete translation from view matrix
	SkyBox::Instance()->update(perspective_projection * glm::mat4(glm::mat3(perspective_view)));
}

void Scene::render() {
	glUseProgram(shaders_animated_model);

	glUniform3f(glGetUniformLocation(shaders_animated_model, "view_pos"), camera.camera_pos.x, camera.camera_pos.y, camera.camera_pos.z);
	glUniform1f(glGetUniformLocation(shaders_animated_model, "material.shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(shaders_animated_model, "material.transparency"), 1.0f);
	// Point Light 1
	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.position"), camera.camera_pos.x, camera.camera_pos.y, camera.camera_pos.z);

	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.specular"), 1.0f, 1.0f, 1.0f);

	glUniform1f(glGetUniformLocation(shaders_animated_model, "point_light.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaders_animated_model, "point_light.linear"), 0.007);	//0.14 0.09  0.07  0.045  0.027  0.022  0.014  0.007  0.0014 -	ðàçíîå ðàññòîÿíèå çàòóõàíèÿ
	glUniform1f(glGetUniformLocation(shaders_animated_model, "point_light.quadratic"), 0.0002);//0.07 0.032 0.017 0.0075 0.0028 0.0019 0.0007 0.0002 0.000007	ðàññòîÿíèå -->

	MVP = perspective_projection * perspective_view * matr_model_1;
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "M_matrix"), 1, GL_FALSE, glm::value_ptr(matr_model_1));
	glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(matr_model_1)));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube));
	model_man.draw(shaders_animated_model);

	MVP = perspective_projection * perspective_view * matr_model_2;
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "M_matrix"), 1, GL_FALSE, glm::value_ptr(matr_model_2));
	glm::mat4 matr_normals_cube2 = glm::mat4(glm::transpose(glm::inverse(matr_model_2)));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube2));
	model_astroboy.draw(shaders_animated_model);
	glUseProgram(0);

	// draw skybox after scene
	SkyBox::Instance()->draw();

	glDepthFunc(GL_ALWAYS);
	// draw text after skybox so that antialiasing for lines works
	// text 2D
	glm::mat4 translate_2d_text = glm::translate(glm::mat4(), glm::vec3(20.0f, 65.0f, 0.0f));
	glm::mat4 scale_2d_text = glm::scale(glm::mat4(), glm::vec3(0.5f, 0.5f, 0.5f));
	TextRendering::Instance()->draw("Buttons: 1,2,3,4 = controls head Agent_1", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_2D * translate_2d_text * scale_2d_text);
	
	translate_2d_text = glm::translate(glm::mat4(), glm::vec3(20.0f, 35.0f, 0.0f));
	TextRendering::Instance()->draw("Buttons: 5,6,7,8,9,0 = change skybox", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_2D * translate_2d_text * scale_2d_text);

	translate_2d_text = glm::translate(glm::mat4(), glm::vec3(20.0f, 5.0f, 0.0f));
	TextRendering::Instance()->draw("Buttons: W, S, A, D, SPACE = move  (ALT + F4 = stop)", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_2D * translate_2d_text * scale_2d_text);

	// text 3D 
	TextRendering::Instance()->draw("Agent_1", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_3D_model_1);
	TextRendering::Instance()->draw("Agent_2", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_3D_model_2);

	glDepthFunc(GL_LESS);
	
}

void Scene::playSound()
{
	if (Mix_PlayingMusic() == 0) //if first song end and play nothing 
		Mix_PlayMusic(music2, 1); // play two time( loop = 1) )

	model_man.playSound();
}

GLuint Scene::loadImageToTexture(const char* image_path)
{
	fipImage img;
	img.load(image_path);
	img.convertTo32Bits();

	std::cout << img.getWidth() << " " << img.getHeight() << std::endl;
	std::cout << img.isGrayscale() << std::endl;
	std::cout << img.getBitsPerPixel() << std::endl;


	int width = img.getWidth();
	int height = img.getHeight();
	int channels = img.getBitsPerPixel();

	GLuint textureID;
	glGenTextures(1, &textureID); 
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.accessPixels());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

