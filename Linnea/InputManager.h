#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL/SDL.h>

class InputManager
{
public:
	InputManager();

	void keyEvent(SDL_KeyboardEvent& evnt);
	void textEvent(SDL_TextInputEvent& evnt);

	void registerKeyStroke(std::string keyStroke, unsigned int key);

	bool isKeyDown(unsigned int key);
	bool isKeyDown(std::string keyStroke);

	bool isMouseKeyDown(unsigned int key);
	bool isLMouseDown();
	bool isRMouseDown();
	bool isMMouseDown();

	void mouseButtonEvent(SDL_MouseButtonEvent& evnt);

	void mouseWheelEvent(SDL_MouseWheelEvent& evnt);
	int dWheel();

	void mouseMotionEvent(SDL_MouseMotionEvent& evnt);
	int mouseX();
	int mouseY();
	bool mouseMoved();

	void clearInput();

private:
	bool _mouseButtonStates[3];
	std::unordered_map<unsigned int, bool> _keyStates;
	std::unordered_map<std::string, unsigned int> _keyStrokes;

	int _dWheel;
	int _mouseX;
	int _mouseY;
	int _mouseRelX;
	int _mouseRelY;

	char _symbol;
	int _key;
};

