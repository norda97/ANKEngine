#pragma once

#include <vector>
#include <thread>
#include <stack>
#include <mutex>

class ANKThreadPool
{
private:
	ANKThreadPool() = default;
	ANKThreadPool(const ANKThreadPool& other) = delete;
	ANKThreadPool(ANKThreadPool&& other) = delete;
public:
	~ANKThreadPool() = default;

	static bool Init();

	static void QueueJob(std::function<void()> job);

	static void Release();

private:
	static void HandleJobs();
private:
	static bool		s_Initilized;
	static bool		s_Terminate;
	static uint32_t	s_SupportedThreads;
	static std::vector<std::thread> s_ThreadPool;

	static std::stack<std::function<void()>> s_JobQueue;

	static std::mutex s_JobMutex;
	static std::mutex s_PoolMutex;
	static std::condition_variable s_WaitCondition;
};