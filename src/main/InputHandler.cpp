#include "InputHandler.h"
#include "renderer/WindowHandler.h"


InputHandler::InputHandler()
{

}


InputHandler::~InputHandler()
{
}

void InputHandler::init()
{

}

void InputHandler::grabMouse(bool grabbed)
{
	_mouseGrabbed = grabbed;
	
	if (grabbed)
	{
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
	}
}

void InputHandler::reset(double delta)
{
	if (keyPressed(SDL_SCANCODE_ESCAPE))
	{
		grabMouse(!_mouseGrabbed);
	}

	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		pressedKeys[i] = false;
		releasedKeys[i] = false;
	}

	for (int i = 0; i < MOUSE_SIZE; i++)
	{
		pressedButtons[i] = false;
		releasedButtons[i] = false;
	}
	if (_mouseGrabbed)
	{
		WindowHandler::setMousePosition(WindowHandler::getWidth() / 2, WindowHandler::getHeight() / 2);
		mouseVelocity = (currentMousePosition - glm::vec2(WindowHandler::getWidth() / 2, WindowHandler::getHeight() / 2)) / float(delta);
	}
	else
	{
		mouseVelocity = (currentMousePosition - lastMousePosition) / float(delta);
	}
	lastMousePosition = currentMousePosition;
}

void InputHandler::update(SDL_Event event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (!activeKeys[event.key.keysym.scancode])
			pressedKeys[event.key.keysym.scancode] = true;

		activeKeys[event.key.keysym.scancode] = true;
	}

	if (event.type == SDL_KEYUP)
	{
		releasedKeys[event.key.keysym.scancode] = true;
		activeKeys[event.key.keysym.scancode] = false;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		if (!activeButtons[event.button.button])
			pressedButtons[event.button.button] = true;

		activeButtons[event.button.button] = true;
	}

	if (event.type == SDL_MOUSEBUTTONUP)
	{
		releasedButtons[event.button.button] = true;
		activeButtons[event.button.button] = false;
	}

	if (event.type == SDL_MOUSEMOTION)
	{
		currentMousePosition = glm::vec2(event.motion.x, event.motion.y);
	}
}

bool InputHandler::keyPressed(SDL_Scancode key)
{
	return pressedKeys[key];
}

bool InputHandler::keyDown(SDL_Scancode key)
{
	return activeKeys[key];
}

bool InputHandler::keyReleased(SDL_Scancode key)
{
	return releasedKeys[key];
}

bool InputHandler::mouseButtonPressed(int button)
{
	return pressedButtons[button];
}

bool InputHandler::mouseButtonDown(int button)
{
	return activeButtons[button];
}

bool InputHandler::mouseButtonReleased(int button)
{
	return releasedButtons[button];
}

bool InputHandler::mouseButtonDoubleClicked(int button)
{
	return doubleClickedButtons[button];
}

glm::vec2 InputHandler::getMousePosition() const
{
	return currentMousePosition;
}

glm::vec2 InputHandler::getMouseVelocity() const
{
	return mouseVelocity;
}
