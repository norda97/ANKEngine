#pragma once

#include <vector>
#include <thread>

class ANKThreadPool
{
private:
	ANKThreadPool() = default;
	ANKThreadPool(const ANKThreadPool& other) = delete;
	ANKThreadPool(ANKThreadPool&& other) = delete;
public:
	~ANKThreadPool() = default;

	static bool Init();

	static void Release();

private:
	static bool		s_Initilized;
	static uint32_t	s_SupportedThreads;
	static uint32_t	s_ActiveThreads;
	static std::vector<std::thread> s_Threads;
};