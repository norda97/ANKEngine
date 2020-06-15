#pragma once

#include "Core/IO/KeyInputs.h"

class Input
{
public:
	~Input();

	static Input& get();


	bool keyPress(int key)		const;
	bool keyPressed(int key)	const;
	bool keyReleased(int key)	const;

	void registerKeyUp(int64_t key, int64_t keyInfo);
	void registerKeyDown(int64_t key, int64_t keyInfo);

private:
	Input() {};
	Input(const Input& other) = delete;
	Input(Input&& other) = delete;

	std::unordered_map<int64_t, int64_t> keyMap;
};