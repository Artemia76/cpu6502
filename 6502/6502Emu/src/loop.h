#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

class CLoop;

class CEvent
{
    friend CLoop;
    protected:
    virtual void OnUpdate()=0;
};


typedef std::chrono::high_resolution_clock hrc;
typedef std::chrono::microseconds period;
typedef std::vector<CEvent*> v_subscribers;

class CLoop
{
    public:
                    CLoop ();
                    ~CLoop ();
    void            Start (int pPeriod=10);
    void            Stop ();
    void            WaitEnd();
    void            Subscribe (CEvent* pSubscriber);
    void            UnSubscribe (CEvent* pSubscriber);

    private:
    std::thread*    m_thread;
    hrc::time_point m_start;
    hrc::time_point m_end;
    period          m_period;
    std::atomic<bool> m_running;
    v_subscribers   m_subscribers;
    std::mutex      m_mutex;
    void            mainLoop();
};