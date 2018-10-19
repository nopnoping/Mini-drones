#ifndef _TASK_SCHEDULER_H_
#define _TASK_SCHEDULER_H_
#include "include.h"

//根据数组长度，判断线程数量
#define TASK_NUM (sizeof(sched_tasks)/sizeof(sched_task_t))

typedef struct
{
	void(*task_func)(void);
	uint16_t rate_hz;
	uint16_t interval_ticks;
	uint32_t last_run;
}sched_task_t;
extern sched_task_t sched_tasks[];

void Loop_1000Hz(void);
void Loop_500Hz(void);
void Loop_200Hz(void);
void Loop_100Hz(void);
void Loop_50Hz(void);
void Loop_20Hz(void);
void Loop_10Hz(void);
void Loop_2Hz(void);

extern void Scheduler_Setup(void);
extern void Scheduler_Run(void);

#endif
