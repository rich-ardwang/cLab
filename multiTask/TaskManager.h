#include <map>
#include <set>
#include <vector>
#include "timer.h"

struct data
{
    data(double a, double b) : d1(a), d2(b) {}

    double     d1;
    double     d2;
};

struct CTask
{
    CTask(int span, data *p) : timeSpan(span), dp(p) {}
    ~CTask()
    {
        if (dp)
        {
            delete dp;
            dp = nullptr;
        }
    }

    int     timeSpan;
    data    *dp;
};

void handle_task(int spanId)
{
    //todo
}

class CTaskHandler
{
public:
    inline CTaskHandler(int timeSpan, std::vector<CTask*> taskArray)
    {
        m_taskArray = taskArray;
        m_timer = new lava::utils::timer;
        m_timer->start(1000 * (timeSpan), std::bind(handle_task, timeSpan));
    }
    ~CTaskHandler();

public:

private:
    std::vector<CTask*>     m_taskArray;
    lava::utils::timer*     m_timer;
};

class CTaskManager
{
public:
    CTaskManager();
    ~CTaskManager();

public:
    void clearTask();
    void addTask(CTask *pt);
    void removeTask(CTask *pt);
    void replaceTask(CTask *target, CTask *pt);

private:
    void handleTask();

private:
    std::set<int>                          m_timeSpanSet;
    std::map<int,  >    m_taskCenter;
    std::set<>          m_timerSet;
};