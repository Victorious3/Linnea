#include "InputManager.h"

InputManager::InputManager() {
	_mouseButtonStates[0] = false;
	_mouseButtonStates[1] = false;
	_mouseButtonStates[2] = false;

	clearInput();
}

void InputManager::keyEvent(SDL_KeyboardEvent& evnt) {
	_keyStates[evnt.keysym.sym] = evnt.state != 0;
}

void InputManager::textEvent(SDL_TextInputEvent & evnt)
{
	_symbol = evnt.text[0];
}

void InputManager::registerKeyStroke(std::string keyStroke, unsigned int key) {
	_keyStrokes[keyStroke] = key;
}

bool InputManager::isKeyDown(unsigned int key) {
	auto it = _keyStates.find(key);
	if (it != _keyStates.end()) {
		return it->second;
	}
	return false;
}

bool InputManager::isKeyDown(std::string keyStroke) {
	auto it = _keyStrokes.find(keyStroke);
	if (it != _keyStrokes.end()) {
		return isKeyDown(it->second);
	}
	return false;
}

bool InputManager::isMouseKeyDown(unsigned int key) {
	return _mouseButtonStates[key - 1];
}

bool InputManager::isLMouseDown() {
	return _mouseButtonStates[SDL_BUTTON_LEFT];
}

bool InputManager::isRMouseDown() {
	return _mouseButtonStates[SDL_BUTTON_RIGHT];
}

bool InputManager::isMMouseDown() {
	return _mouseButtonStates[SDL_BUTTON_MIDDLE];
}

void InputManager::mouseButtonEvent(SDL_MouseButtonEvent& evnt) {
	unsigned int key = evnt.button;
	// Make sure that we don't catch any weird keys
	if (key - 1 < 2) {
		_mouseButtonStates[key] = evnt.state != 0;
	}
}

void InputManager::mouseWheelEvent(SDL_MouseWheelEvent& evnt) {
	_dWheel = evnt.y;
}

int InputManager::dWheel()
{
	return _dWheel;
}

void InputManager::mouseMotionEvent(SDL_MouseMotionEvent& evnt) {
	_mouseX = evnt.x;
	_mouseY = evnt.y;
	_mouseRelX = evnt.xrel;
	_mouseRelY = evnt.yrel;
}

int InputManager::mouseX()
{
	return _mouseX;
}

int InputManager::mouseY()
{
	return _mouseX;
}

bool InputManager::mouseMoved()
{
	return _mouseX && _mouseY;
}

void InputManager::clearInput() {
	_mouseX = 0;
	_mouseY = 0;
	_mouseRelX = 0;
	_mouseRelY = 0;

	_dWheel = 0;

	_key = 0;
	_symbol = 0;
}