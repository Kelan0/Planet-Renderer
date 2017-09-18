#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <chrono>

#define MOUSE_SIZE 16

class InputHandler
{
private:
	bool pressedKeys[SDL_NUM_SCANCODES];
	bool activeKeys[SDL_NUM_SCANCODES];
	bool releasedKeys[SDL_NUM_SCANCODES];

	bool pressedButtons[MOUSE_SIZE];
	bool activeButtons[MOUSE_SIZE];
	bool releasedButtons[MOUSE_SIZE];
	bool doubleClickedButtons[MOUSE_SIZE];

	glm::vec2 currentMousePosition;
	glm::vec2 lastMousePosition;
	glm::vec2 mouseVelocity;

	std::chrono::high_resolution_clock::time_point lastEvent;
	bool _mouseGrabbed;
public:
	InputHandler();
	~InputHandler();

	void init();

	void grabMouse(bool grabbed);

	void reset(double delta);

	void update(SDL_Event event);

	bool keyPressed(SDL_Scancode key);
	
	bool keyDown(SDL_Scancode key);
	
	bool keyReleased(SDL_Scancode key);

	bool mouseButtonPressed(int button);

	bool mouseButtonDown(int button);

	bool mouseButtonReleased (int button);

	bool mouseButtonDoubleClicked(int button);

	glm::vec2 getMousePosition() const;

	glm::vec2 getMouseVelocity() const;
};

