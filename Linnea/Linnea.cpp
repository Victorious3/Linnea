#include "Linnea.h"

#include <chrono>
#include <iostream>

#include "common.h"
#include "InputManager.h"
#include "Configuration.h"

Linnea* Linnea::_instance = nullptr;

Linnea* Linnea::instance()
{
	return _instance;
}

Linnea::Linnea(string name, unsigned int screenWidth, unsigned int screenHeight) : name(name)
{
	if (_instance != nullptr) throw "Already created an instance!";

	_inputManager = nullptr;
	_logger = nullptr;
	_config = nullptr;

	_window = nullptr;
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;
	_instance = this;
}

Linnea::~Linnea() { finalize(); }

InputManager* Linnea::inputManager()
{
	return _inputManager;
}

spdlog::logger* Linnea::logger()
{
	return _logger;
}

Configuration* Linnea::config() 
{
	return _config;
}

void Linnea::start()
{
	if (_isRunning) return;
	init();
	_isRunning = true;
	run();
}

void Linnea::stop()
{
	_isRunning = false;
}

void Linnea::init()
{
	// Initialize Logger
	vector<spdlog::sink_ptr> sinks = {
		make_shared<spdlog::sinks::stdout_sink_st>(),
		make_shared<spdlog::sinks::daily_file_sink_st>("logs/", "txt", 0, 0)
	};
	_logger = new spdlog::logger("global", sinks.begin(), sinks.end());
	LOG->info("Created Logger");

	// Initializes configuration
	_config = new Configuration("linnea.ini");

	// Initialize input manager
	_inputManager = new InputManager();

	// Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
}

void Linnea::finalize()
{
	_config->save();

	delete _inputManager;
	delete _logger;
	delete _config;
}

void Linnea::processInput()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			stop();
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			_inputManager->keyEvent(evnt.key);
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			_inputManager->mouseButtonEvent(evnt.button);
			break;

		case SDL_TEXTINPUT:
			_inputManager->textEvent(evnt.text);
			break;

		case SDL_MOUSEMOTION:
			_inputManager->mouseMotionEvent(evnt.motion);
			break;

		case SDL_MOUSEWHEEL:
			_inputManager->mouseWheelEvent(evnt.wheel);
			break;
		}
	}
}

void Linnea::run()
{
	_frameCount = 0;
	double unprocessedSeconds = 0;
	long long previousTime = common::time_nano();
	double secondsPerTick = 1 / 20.0;
	int tickCount = 0;

	long long lastTick = previousTime;
	while (_isRunning) {
		long long currentTime = common::time_nano();
		long long passedTime = currentTime - previousTime;
		previousTime = currentTime;
		unprocessedSeconds += passedTime / 1000000000.0;

		while (unprocessedSeconds > secondsPerTick) {
			tick();
			_tickCount++;

			unprocessedSeconds -= secondsPerTick;
			tickCount++;
			if (tickCount % 60 == 0) {
				previousTime += 1000;
				_fps = _frameCount;
				_frameCount = 0;
			}
		}

		_frameCount++;
		currentTime = common::time_nano();
		render(static_cast<float>((currentTime - lastTick) / 1000000000.0 / secondsPerTick));
		lastTick = currentTime;
	}

}

void Linnea::render(float partialTicks)
{
	cout << _tickCount << "," << partialTicks << endl;
}

void Linnea::tick()
{
	processInput();
	// Logic for game update here
	_inputManager->clearInput();
}
