#include "Logger.h"

namespace Logger
{
	void SetConsoleTextColor(int color)
	{
		if (g_ConsoleHandle != INVALID_HANDLE_VALUE)
			SetConsoleTextAttribute(g_ConsoleHandle, color);
	}
}
