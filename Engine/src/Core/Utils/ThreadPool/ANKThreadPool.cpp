#include "pch.h"
#include "ANKThreadPool.h"

bool						ANKThreadPool::s_Initilized = false;
uint32_t					ANKThreadPool::s_ActiveThreads = 0;
uint32_t					ANKThreadPool::s_SupportedThreads = 0;
std::vector<std::thread>	ANKThreadPool::s_Threads;

bool ANKThreadPool::Init()
{
	s_SupportedThreads = std::thread::hardware_concurrency();
	ANK_INFO("ANKThreadPool: Supported Thread %d", s_SupportedThreads);


	s_Initilized = true;
	return s_Initilized;
}

void ANKThreadPool::Release()
{
}
