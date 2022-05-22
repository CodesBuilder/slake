#ifndef __TASK_H__
#define __TASK_H__

typedef struct _SlakeTask SlakeTask;

typedef int (*SlakeTaskProc)();

SlakeTask *slakeCreateTask(SlakeTaskProc proc);
void slakeKillTask(SlakeTask* task);
int slakeIsTaskAlive(SlakeTask* task);
void slakeAwait(SlakeTask* task);
void slakeYield();

#endif
