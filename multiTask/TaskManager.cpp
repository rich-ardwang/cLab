#include "stdafx.h"
#include "TaskManager.h"

CTaskHandler::CTaskHandler(int timeSpan, std::vector<CTask*> *pTaskArray)
{
    m_workFinish = true;
    m_start = false;
    m_timeSpan = timeSpan * 1000;
    m_pTaskArray = pTaskArray;
    m_timer = new lava::utils::timer;
}

CTaskHandler::~CTaskHandler()
{
    if (m_start)
        stop();

    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }

    ReleaseTasks(m_pTaskArray);
}

void CTaskHandler::InsertTask(std::vector<CTask*> *pTaskArray)
{
    if (pTaskArray->empty())
        return;

    std::vector<CTask*> *pNewTasks = nullptr;
    CloneTasks(m_pTaskArray, pNewTasks);
    auto iter = pTaskArray->begin();
    for (; iter != pTaskArray->end(); ++iter)
    {
        pNewTasks->push_back(*iter);
    }

    std::vector<CTask*> *pOldTasks = m_pTaskArray;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_pTaskArray = pNewTasks;
    }
    ReleaseTasks(pOldTasks);
}

void CTaskHandler::RemoveTask(std::vector<CTask*> *pTaskArray)
{
    if (pTaskArray->empty())
        return;

    std::vector<CTask*> *pNewTasks = nullptr;
    CloneTasks(m_pTaskArray, pNewTasks);
    auto iter = pTaskArray->begin();
    for (; iter != pTaskArray->end(); ++iter)
    {
        if (*iter)
        {
            delete (*iter);
            (*iter) = nullptr;
        }
        iter = pNewTasks->erase(iter);
    }

    std::vector<CTask*> *pOldTasks = m_pTaskArray;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_pTaskArray = pNewTasks;
    }
    ReleaseTasks(pOldTasks);
}

void CTaskHandler::start()
{
    if (m_start)
        return;

    if (m_timer)
    {
        m_timer->start(m_timeSpan, std::bind(&CTaskHandler::handle_task, this));
        m_start = true;
    }
}

void CTaskHandler::stop()
{
    if (!m_start)
        return;

    if (m_timer)
    {
        m_timer->expire();
        while (true)
        {
            if (m_workFinish)
            {
                
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(500));
        }
    }
    m_start = false;
}

void CTaskHandler::CloneTasks(std::vector<CTask*> *target, std::vector<CTask*> *des)
{
    des = nullptr;
    if (target->empty())
        return;

    des = new std::vector<CTask*>();
    auto iter = target->begin();
    for (; iter != target->end(); ++iter)
    {
        if (*iter)
        {
            CTask *p = new CTask(**iter);
            des->push_back(p);
        }
    }
}

void CTaskHandler::ReleaseTasks(std::vector<CTask*> *p)
{
    if (p)
    {
        auto iter = p->begin();
        for (; iter != p->end(); ++iter)
        {
            if (*iter)
            {
                delete (*iter);
                (*iter) = nullptr;
            }
        }
        p->clear();
        delete p;
        p = nullptr;
    }
}

void CTaskHandler::handle_task()
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_workFinish = false;
        m_pTaskArray;
        m_workFinish = true;
    }
}

void CTaskManager::addTask(std::vector<CTask*> *pt)
{
}

void CTaskManager::removeTask(std::vector<CTask*> *pt)
{

}

void CTaskManager::replaceTask(CTask *target, CTask *pt)
{

}