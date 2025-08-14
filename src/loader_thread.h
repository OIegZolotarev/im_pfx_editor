#pragma once

#include "common.h"
#include <SDL2/SDL_thread.h>
#include <functional>
#include <queue>

#define NUM_THREADS 1

// Forward-declaration
class ITask;
class LoaderTask;
class LoaderThread;

enum class TaskStepResultType
{
    FinishedSuccesfully = 0,
    FinishedWithFailure,
    StepPerformed,
    None
};

typedef std::function<TaskStepResultType()>                         pfnTaskRoutine;
typedef std::function<void(ITask *task, TaskStepResultType result)> pfnTaskCompletedCallback;

typedef struct taskprogressinfo_s
{
    bool hasBackgroundTasks = false;

    std::string operationDescription;
    std::string elementDescription;

    size_t numTasksRemaining = 0;

    size_t currentTaskTotalSteps     = 0;
    size_t currentTaskPerformedSteps = 0;

} taskprogressinfo_t;

typedef struct threadInfo_s
{
    char                name[32];
    bool                alive = true;
    SDL_mutex *         mutex;
    SDL_Thread *        thread_handle;
    std::queue<ITask *> tasks;

    size_t remainingTasks;
    size_t currentTaskTotalSteps     = 0;
    size_t currentTaskPerformedSteps = 0;

    std::string taskDescription;
    std::string taskElementDescription;

    LoaderThread *loaderThread;
} threadInfo_t;

class ITaskStepResult
{
protected:
    TaskStepResultType m_StepResult = TaskStepResultType::None;

    std::string m_strElementDescription = "";

public:
    ITaskStepResult(TaskStepResultType r) : m_StepResult(r)
    {
    }

    TaskStepResultType GetType()
    {
        return m_StepResult;
    }

    virtual void ExecuteOnCompletion(){};

    std::string &Description();

    virtual bool NeedEndCallback()
    {
        return false;
    }
};

class ITask
{
protected:
    TaskStepResultType m_TaskResult = TaskStepResultType::None;

    size_t m_nTotalSteps     = 0;
    size_t m_nPerformedSteps = 0;

    std::string m_strDescription;

public:
    virtual ~ITask() = default;

    // ��������� ��� ������
    // ���������� �� ������ �������
    virtual ITaskStepResult *ExecuteStep(LoaderThread *loaderThread) = 0;

    virtual void OnCompletion() = 0;

    size_t TotalSteps()
    {
        return m_nTotalSteps;
    }
    size_t PerformedSteps()
    {
        return m_nPerformedSteps;
    }
    std::string &Description()
    {
        return m_strDescription;
    }

    void Schedule();
};

// class LoaderTask: public ITask
//{
//	pfnTaskRoutine m_fTaskRoutine;
//	pfnTaskCompletedCallback m_fEndRoutine;
//	const char* m_strDescription;
// public:
//	LoaderTask(const char* description, const char* elementDescription, const pfnTaskRoutine taskRoutine, const
//pfnTaskCompletedCallback endRoutine);
//
//	virtual bool ExecuteStep(LoaderThread* loaderThread) override;
//	virtual void OnCompletion() override;
//
// private:
//	const char* m_strElementDescription;
//};

class LoaderThread
{
    SDL_mutex *  m_MainThreadMutex = 0;
    threadInfo_t m_Threads[NUM_THREADS];

    std::queue<ITaskStepResult *> m_FinishedTasks;

    void InitThreads();
    LoaderThread();

public:
    ~LoaderThread();
    static LoaderThread *Instance();

    void Start();
    void Stop();

    void ScheduleTask(ITask *pTask);
    void FillTaskProgressInfo(taskprogressinfo_t *infoDest);

    void ScheduleEndCallback(ITaskStepResult *task);
    void ExecuteEndCallbacks(size_t maxCallbacks);

private:
    void DestroyThreads();
};

int loaderThreadFunction(void *data);
