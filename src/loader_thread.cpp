#include "loader_thread.h"
#include <SDL2/SDL_timer.h>

int loaderThreadFunction(void* data)
{
	threadInfo_t* info = (threadInfo_t*)data;

	auto updateInfo = [&](ITask* task, ITaskStepResult* result)
	{
		info->currentTaskTotalSteps = task->TotalSteps();
		info->currentTaskPerformedSteps = task->PerformedSteps();

		info->taskElementDescription = result->Description();
		info->taskDescription = task->Description();
	};

	while (info->alive)
	{
		SDL_LockMutex(info->mutex);

		if (info->tasks.empty())
		{
			SDL_UnlockMutex(info->mutex);
			SDL_Delay(0);
			continue;
		}

		auto task = info->tasks.front();
		auto stepResult = task->ExecuteStep(info->loaderThread);

		if (stepResult)
		{
			switch (stepResult->GetType())
			{
			case TaskStepResultType::FinishedSuccesfully:
				info->tasks.pop();
				delete task;
				break;
			case TaskStepResultType::FinishedWithFailure:
				info->tasks.pop();
				delete task;
				break;
			case TaskStepResultType::StepPerformed:
				updateInfo(task, stepResult);
				break;
			default:
				break;
			}

			if (stepResult->NeedEndCallback())
				info->loaderThread->ScheduleEndCallback(stepResult);
			else
				delete stepResult;
		}

		info->remainingTasks = info->tasks.size();

		SDL_UnlockMutex(info->mutex);
		SDL_Delay(0);
	}

	return 0;
}

void LoaderThread::InitThreads()
{
	for (int i = 0; i < NUM_THREADS; i++)
	{
		snprintf(m_Threads[i].name, sizeof(m_Threads[i].name),"Loader thread #%d", i + 1);

		m_Threads[i].loaderThread = this;
		m_Threads[i].mutex = SDL_CreateMutex();
		m_Threads[i].thread_handle = SDL_CreateThread(loaderThreadFunction, m_Threads[i].name, &m_Threads[i]);
		
	}
}

LoaderThread::LoaderThread()
{

}

LoaderThread::~LoaderThread()
{
}

LoaderThread* LoaderThread::Instance()
{
	static LoaderThread* sInstance = new LoaderThread;
	return sInstance;
}

void LoaderThread::Start()
{
	InitThreads();
	m_MainThreadMutex = SDL_CreateMutex();
}

void LoaderThread::Stop()
{
	DestroyThreads();
	
	SDL_DestroyMutex(m_MainThreadMutex);
	m_MainThreadMutex = nullptr;
}

void LoaderThread::ScheduleTask(ITask * pTask)
{
	int bestThread = 0;
	size_t minTasks = 99999999;

	for (int i = 0; i < NUM_THREADS; i++)
	{
		if (m_Threads[i].remainingTasks < minTasks)
		{
			bestThread = i;
			minTasks = m_Threads[i].remainingTasks;
		}
	}

	SDL_LockMutex(m_Threads[bestThread].mutex);
	m_Threads[bestThread].tasks.push(pTask);
	SDL_UnlockMutex(m_Threads[bestThread].mutex);
}

void LoaderThread::FillTaskProgressInfo(taskprogressinfo_t* infoDest)
{
	int threadIndex = 0;

	threadInfo_t* info = &m_Threads[threadIndex];

	//SDL_LockMutex(info->mutex);

	infoDest->numTasksRemaining = info->remainingTasks;	
	infoDest->hasBackgroundTasks = info->remainingTasks > 0;

	infoDest->operationDescription = info->taskDescription;
	infoDest->elementDescription = info->taskElementDescription;

	infoDest->currentTaskTotalSteps = info->currentTaskTotalSteps;
	infoDest->currentTaskPerformedSteps = info->currentTaskPerformedSteps;

	//SDL_UnlockMutex(info->mutex);
}

void LoaderThread::ScheduleEndCallback(ITaskStepResult* task)
{
	SDL_LockMutex(m_MainThreadMutex);	
	m_FinishedTasks.push(task);
	SDL_UnlockMutex(m_MainThreadMutex);
}

void LoaderThread::ExecuteEndCallbacks(size_t maxCallbacks)
{
	

	size_t nRemaining = maxCallbacks;

	if (maxCallbacks == 0)
		nRemaining = m_FinishedTasks.size();

	while (m_FinishedTasks.size() > 0 && nRemaining > 0)
	{
        SDL_LockMutex(m_MainThreadMutex);
		auto stepInfo = m_FinishedTasks.front();
		m_FinishedTasks.pop();
        SDL_UnlockMutex(m_MainThreadMutex);

		stepInfo->ExecuteOnCompletion();
		delete stepInfo;
		
	}

	
}

void LoaderThread::DestroyThreads()
{
	for (int i = 0; i < NUM_THREADS; i++)
	{
		m_Threads[i].alive = false;
		SDL_WaitThread(m_Threads[i].thread_handle, nullptr);
	}
}

std::string& ITaskStepResult::Description()
{
	return m_strElementDescription;
}

void ITask::Schedule()
{
	LoaderThread::Instance()->ScheduleTask(this);
}
