#pragma once

#include <string>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

using namespace std;

// Forward declarations
class InputManager;
class Configuration;

class Linnea
{
public:

	// instance;
	static Linnea* instance();

	Linnea(string name, unsigned int screenWidth = 800, unsigned int screenHeight = 600);
	~Linnea();

	InputManager* inputManager();

	spdlog::logger* logger();

	Configuration* config();

	void start();

	void stop();

	const string name;

private:

	// instance
	static Linnea* _instance;

	void init();

	void run();

	void render(float partialTicks);

	void tick();

	void finalize();

	void processInput();

	//Exit flag
	bool _isRunning = false;

	// Window and resolution
	SDL_Window* _window;
	unsigned int _screenWidth;
	unsigned int _screenHeight;

	// fps counter and frame counter
	int _fps;
	int _frameCount;
	int _tickCount;

	// input manager
	InputManager* _inputManager;

	// logger
	spdlog::logger* _logger;

	// global configuration file
	Configuration* _config;
};

