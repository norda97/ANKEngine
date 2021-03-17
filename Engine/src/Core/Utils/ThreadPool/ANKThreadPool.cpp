#include "pch.h"
#include "ANKThreadPool.h"


bool								ANKThreadPool::s_Initilized = false;
bool								ANKThreadPool::s_Terminate = false;
uint32_t							ANKThreadPool::s_SupportedThreads = 0;

std::mutex							ANKThreadPool::s_JobMutex;
std::mutex							ANKThreadPool::s_PoolMutex;
std::condition_variable				ANKThreadPool::s_WaitCondition;

std::vector<std::thread>			ANKThreadPool::s_ThreadPool;
std::stack<std::function<void()>>	ANKThreadPool::s_JobQueue;

bool ANKThreadPool::Init()
{
	s_SupportedThreads = std::thread::hardware_concurrency();
	ANK_INFO("ANKThreadPool: Supported Threads %d", s_SupportedThreads);

	if (s_SupportedThreads == 0)
		return false;

	for (size_t i = 0; i < s_SupportedThreads; i++)
	{
		s_ThreadPool.push_back(std::thread(HandleJobs));
	}

	s_Initilized = true;
	return s_Initilized;
}

void ANKThreadPool::HandleJobs()
{
	if (s_Initilized)
	{
		while (true)
		{
			std::function<void()> job;
			{
				std::unique_lock<std::mutex> lock(s_JobMutex);

				s_WaitCondition.wait(lock, [] { return !s_JobQueue.empty() || s_Terminate; });

				if (s_Terminate)
					return;

				job = s_JobQueue.top();
				s_JobQueue.pop();
			}
			job();
		}

	}
}


void ANKThreadPool::QueueJob(std::function<void()> job)
{
	{
		std::unique_lock<std::mutex> lock(s_JobMutex);
		s_JobQueue.push(job);
	}
	s_WaitCondition.notify_one();
}

void ANKThreadPool::Release()
{
	// Wait for threads to finish
	if (s_Initilized)
	{
		std::unique_lock<std::mutex> lock(s_PoolMutex);
		s_Terminate = true;
	}

	s_WaitCondition.notify_all(); // wake up all threads.

	// Join all threads.
	for (std::thread& every_thread : s_ThreadPool)
	{
		every_thread.join();
		ANK_INFO("Joined thread!");
	}

	s_ThreadPool.clear();
	s_Initilized = false;
}
