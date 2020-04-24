#include "TaskManager.h"

void CTaskManager::addTask(CTask *pt)
{
    if (!pt)
        return;

    auto iter = m_timeSpanSet.find(pt->timeSpan);
    if (iter == m_timeSpanSet.end())
    {
        m_timeSpanSet.insert(pt->timeSpan);
        lava::utils::timer* tp = new lava::utils::timer;
        m_timerSet.insert(tp);
        tp->start(1000 * (pt->timeSpan), std::bind(handle_task, pt->timeSpan));
    }
    
    
}
