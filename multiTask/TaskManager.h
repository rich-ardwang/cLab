#include <map>
#include <set>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include "timer.h"

struct recordData
{
    recordData(double a, double b) : d1(a), d2(b) {}

    double     d1;
    double     d2;
};

struct CTask
{
    CTask(const int span, recordData *p) : timeSpan(span), rdp(p) {}
    inline CTask(const CTask &task)
    {
        this->timeSpan = task.timeSpan;
        if (task.rdp)
        {
            this->rdp = new recordData(*task.rdp);
        }
        else
            this->rdp = nullptr;
    }
    inline ~CTask()
    {
        if (rdp)
        {
            delete rdp;
            rdp = nullptr;
        }
    }

    int            timeSpan;
    recordData*    rdp;
};

class CTaskHandler
{
public:
    CTaskHandler(int timeSpan, std::vector<CTask*> *pTaskArray);
    ~CTaskHandler();

public:
    void InsertTask(std::vector<CTask*> *pTaskArray);
    void RemoveTask(std::vector<CTask*> *pTaskArray);
    void start();
    void stop();

private:
    void CloneTasks(std::vector<CTask*> *target, std::vector<CTask*> *des);
    void ReleaseTasks(std::vector<CTask*> *p);
    void handle_task();

private:
    int                     m_timeSpan;
    bool                    m_workFinish;
    bool                    m_start;
    std::vector<CTask*>*    m_pTaskArray;
    lava::utils::timer*     m_timer;
    std::mutex              m_mutex;
};

class CTaskManager
{
public:
    CTaskManager() {}
    ~CTaskManager() {}

public:
    void clearTask() {}
    void addTask(std::vector<CTask*> *pt);
    void removeTask(std::vector<CTask*> *pt);
    void replaceTask(CTask *target, CTask *pt);

private:
    std::map<int, CTaskHandler*>    m_taskCenter;
};
