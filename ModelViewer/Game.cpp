#include "Game.h"
#include "InputHandler.h"
//#include "GL/glew.h"
//#include "GL/freeglut.h"
#include "SDL/SDL_mixer.h"
#include <glad/glad.h>

#include <iostream>
#include <assert.h>

Game::Game()
{
	std::cout << "konstructor Game()" << std::endl;

	screen_width = 1024;
	screen_height = 768;

	running = true;
}


Game::~Game()
{

}

void Game::init()
{
	assert(2 < 5);

	/*
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	*/
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);


	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// SET ATTRIBUTE ONLY after initialize
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // on antialiasing sdl
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); //subsamples for each pixel

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // set to 1 to require hardware acceleration

		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode(0, &dm); // if resolution change in SDL, this function will return the previous native display mode !!!
		screen_width = 1024; //dm.w;
		screen_height = 576; //dm.h;
		std::cout << dm.w << "    " << dm.h << std::endl;

		window = SDL_CreateWindow("Zero",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			screen_width, screen_height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | 0);

		//SDL_SetWindowOpacity(window, 0.0f); // make window transparent ( prozra4noe )

		if (window != 0)
		{
			glcontext = SDL_GL_CreateContext(window);
			std::cout << "SDL init " << std::endl;
		}
	}
	else
	{
		std::cout << "window create error " << std::endl;
		return; // sdl could not initialize
	}
	SDL_SetWindowFullscreen(window, 0); // переключат?оконны?/ полноэкранны?режи??обратн?бе?потери контекст?GL 

	//glewExperimental = GL_TRUE; //вс?расширен? ?действительным?точкам?вход?буду?выставлены.
	//if (glewInit())// if all is init return 0 !! if not init return 1 and if() run
	//{
	//	std::cout << "glew init error !" << std::endl;
	//}
	if (!gladLoadGL()) {
		printf("Something went wrong!\n");
		exit(-1);
	}
	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	const GLubyte * version = glGetString(GL_VERSION);

										// MIX_DEFAULT_FREQUENCY = sample rate = frequensy = speed playing
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4069) == -1)
		cout << "Mixer NOT init !!" << endl;
	Mix_VolumeMusic(1);

	glViewport(0, 0, screen_width, screen_height);
	glEnable(GL_DEPTH_TEST); 

	int buffer;
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &buffer);
	printf("SDL_GL_DOUBLEBUFFER: %d \n", buffer);

	triangle.init();

}

void Game::handleEvents()
{
	InputHandler::Instance()->updateEvent();
}

void Game::update()
{
	triangle.update();
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	triangle.render();


	glFlush();
	//glFinish(); 
	SDL_GL_SwapWindow(window);
	 
}

void Game::playSound()
{
	triangle.playSound();
}

void Game::clean()
{
	std::cout << "cleaning " << std::endl;
	running = false;
	SDL_DestroyWindow(window);
	SDL_Quit();

}