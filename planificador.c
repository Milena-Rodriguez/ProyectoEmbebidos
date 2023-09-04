#include "planificador.h"

/* Lista interna de tareas a ejecutarse */
struct Task _scheduled_tasks[MAX_TASKS];

struct Task *SchedulerAddTask(tTime now, tTime timeFromNow, tTime timeBetweenRuns, TaskCallback callback)
{
  for (int i = 0; i < MAX_TASKS; i++)
    if (!_scheduled_tasks[i].valid) {
      _scheduled_tasks[i].runNextAt = now + timeFromNow;
      _scheduled_tasks[i].callback = callback;
      _scheduled_tasks[i].timeBetweenRuns = timeBetweenRuns;
      _scheduled_tasks[i].enabled = false;
      _scheduled_tasks[i].valid = true;
      return &_scheduled_tasks[i];
    }

  return NULL;
}

void TaskResetPeriodic(struct Task *t)
{
  t->runNextAt = TimeNow() + t->timeBetweenRuns;
}

void TaskSetNextTime(struct Task *t, tTime timeFromNow)
{
  t->runNextAt = TimeNow() + timeFromNow;
}

void TaskDisable(struct Task* t)
{
  t->enabled = false;
}

void TaskEnable(struct Task* t)
{
  t->enabled = true;
}

void SchedulerRun()
{
  struct Task *t;
  tTime now = TimeNow();

  for (int i = 0; i < MAX_TASKS; i++) {
    t = &_scheduled_tasks[i];
    if (t->valid) {
      if ((now >= t->runNextAt) && t->enabled) {
        t->callback(t);
        if (t->timeBetweenRuns > 0)
          TaskResetPeriodic(t);
        else {
          t->valid = false;
        }
      }
    }
  }
}

tTime TimeNow()
{

 // tTime now = millis;

  return 0;
}

tTime TimePassed(tTime since)
{
  tTime now = TimeNow();

  return now - since;
}