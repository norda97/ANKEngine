#pragma once

namespace Logger
{
	static HANDLE g_ConsoleHandle = nullptr;

	constexpr const int RED = 12;
	constexpr const int GREEN = 10;
	constexpr const int YELLOW = 14;
	constexpr const int WHITE = 15;

	inline void SetConsoleTextColor(int color) 
	{
		if (g_ConsoleHandle != INVALID_HANDLE_VALUE)
			SetConsoleTextAttribute(g_ConsoleHandle, color);
	}

#ifdef ANK_DEBUG
#include <crtdbg.h>
#define LOG_ERROR(...) SetConsoleTextColor(RED); printf("[ERROR] "); printf(__VA_ARGS__); printf("\n"); SetConsoleTextColor(WHITE);
#define LOG_WARNING(...)  SetConsoleTextColor(YELLOW); printf("[WARNING] "); printf(__VA_ARGS__); printf("\n"); SetConsoleTextColor(WHITE);
#define LOG_INFO(...)   SetConsoleTextColor(GREEN); printf("[INFO] "); printf(__VA_ARGS__); printf("\n"); SetConsoleTextColor(WHITE);
#define ANK_ASSERT(exp, ...) assert(exp && __VA_ARGS__);
#define _CRT_SECURE_NO_WARNINGS
#else

#define LOG_ERROR(...)
#define LOG_WARNING(...)
#define LOG_INFO(...)
#define ANK_ASSERT(exp, ...) {exp;}
#endif
}
