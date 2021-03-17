#include "pch.h"
#include "Input.h"

#include "assimp/Importer.hpp"

Input::~Input()
{

}

Input& Input::Get()
{
	static Input i;

	return i;
}

bool Input::keyPress(int key) const
{
	auto it = this->keyMap.find(key);
	if (it != this->keyMap.end()) {
		int64_t presses = (it->second >> 30) & 1;
		if ((it->second & 0xFFFF) > 0) {
			LOG_INFO("Presses: %d", presses);
			return true;
		}
	}

	return false;
}

bool Input::keyPressed(int key) const
{
	if (this->keyMap.find(key) == this->keyMap.end())
		return false;

	return true;
}

bool Input::keyReleased(int key) const
{
	// Implement
	return false;
}

void Input::registerKeyUp(int64_t key, int64_t keyInfo)
{
	if (this->keyMap.find(key) != this->keyMap.end())
		this->keyMap.erase(key); 
}

void Input::registerKeyDown(int64_t key, int64_t keyInfo)
{
	this->keyMap[key] = keyInfo;
}
