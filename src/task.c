#include "task.h"
#include <util/list.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
typedef struct _SlakeTask
{
	HANDLE hThread;
}SlakeTask;

static LPVOID WINAPI slakeTaskWrap(LPVOID param)
{
	SlakeTaskProc proc=param;
	return proc();
}
#endif

SlakeTask *slakeCreateTask(SlakeTaskProc proc)
{
	#ifdef _WIN32
	SlakeTask* t=malloc(sizeof(SlakeTask));
	if(!t)
		return NULL;

	t->hThread=CreateThread(NULL,0,slakeTaskWrap,proc,0,NULL);
	return t;
	#endif
}

void slakeKillTask(SlakeTask* task)
{
	#ifdef _WIN32
	TerminateThread(task->hThread, 0xffffffff);
	CloseHandle(task->hThread);
	#endif
}

int slakeIsTaskAlive(SlakeTask* task)
{
	#ifdef _WIN32
	#endif
}

void slakeAwait(SlakeTask* task)
{
	#ifdef _WIN32
	WaitForSingleObject(task->hThread,INFINITE);
	#endif
}

void slakeYield()
{
	#ifdef _WIN32
	SwitchToThread();
	#endif
}
