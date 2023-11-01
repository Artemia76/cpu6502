#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

typedef std::chrono::high_resolution_clock hrc;
typedef std::chrono::microseconds period;

class CLoop;

/**
 * @brief Abstract class for process callback
 * 
 */
class CProcessEvent
{
    friend CLoop;
    public:
                CProcessEvent(CLoop& pParent);
                ~CProcessEvent();
    protected:
    virtual void OnProcess(const period& pInterval)=0;

    private:
    CLoop&      m_parent;
};
typedef std::vector<CProcessEvent*> v_subscribers;

/**
 * @brief Loop Class Ensure Main loop management 
 * 
 * 
 */
class CLoop
{
    public:
                    CLoop ();
                    ~CLoop ();
    void            Start (int pPeriod=10);
    void            Stop ();
    void            WaitEnd();
    void            Subscribe (CProcessEvent* pSubscriber);
    void            UnSubscribe (CProcessEvent* pSubscriber);

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