// 
// 
// 

#include "Task.h"
#include "Event.h"
#include "MicroTasks.h"

unsigned long MicroTasksClass::WaitForEvent = (1 << 31);
unsigned long MicroTasksClass::WaitForMessage = (1 << 30);

unsigned long MicroTasksClass::WaitForMask = MicroTasksClass::WaitForEvent | WaitForMessage;

unsigned long MicroTasksClass::Infinate = ~MicroTasksClass::WaitForMask;

MicroTasksClass::MicroTasksClass()
{
}

void MicroTasksClass::init()
{
}

void MicroTasksClass::update()
{
  Event *oNextEvent;
  Task *oNextTask;

  // Any events triggered?
  for (Event *oEvent = (Event *)Event::oEvents.GetFirst(); oEvent; oEvent = oNextEvent)
  {
    oNextEvent = (Event *)oEvent->GetNext();

    if (oEvent->triggered)
    {
      for (Task *oTask = (Task *)oEvent->oClients.GetFirst(); oTask; oTask = oNextTask)
      {
        // Keep a pointer to the next task in case this on is stopped
        oNextTask = (Task *)oTask->GetNext();

        WakeTask(oTask, WakeReason_Event);
      }

      oEvent->triggered = 0;
    }
  }

  // Any tasks waiting to be woken
  for (Task *oTask = (Task *)oTasks.GetFirst(); oTask; oTask = oNextTask)
  {
    // Keep a pointer to the next task in case this on is stopped
    oNextTask = (Task *)oTask->GetNext();

    if (oTask->ulNextLoop <= millis()) {
      WakeTask(oTask, WakeReason_Scheduled);
    }
  }
}

void MicroTasksClass::WakeTask(Task * oTask, WakeReason eReason)
{
  unsigned long ulDelay = oTask->loop(eReason);

  oTask->uiFlags = ulDelay & MicroTasks.WaitForMask;
  if (MicroTasks.Infinate == (ulDelay & ~MicroTasks.WaitForMask)) {
    oTask->ulNextLoop = 0xFFFFFFFF;
  } else {
    oTask->ulNextLoop += (ulDelay & ~MicroTasks.WaitForMask);
  }
}

void MicroTasksClass::startTask(Task *oTask)
{
  oTasks.Add(oTask);
  oTask->setup();
}

void MicroTasksClass::stopTask(Task *oTask)
{
  oTasks.Remove(oTask);
}

MicroTasksClass MicroTasks;

