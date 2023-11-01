#include "loop.h"
#include <algorithm>

CProcessEvent::CProcessEvent(CLoop& pParent) : m_parent(pParent)
{
    m_parent.Subscribe(this);
}

CProcessEvent::~CProcessEvent()
{
    m_parent.UnSubscribe(this);
}

CLoop::CLoop ()
{
    m_running=false;
    m_thread = nullptr;
}

CLoop::~CLoop ()
{
    if (m_running) Stop();
}

void CLoop::Start(int pPeriod)
{
    m_period = std::chrono::microseconds(pPeriod*1000);
    if ((!m_running) && (m_thread==nullptr))
    {
        m_running = true;
        m_thread= new std::thread(&CLoop::mainLoop, this);
    }
}

void CLoop::Stop()
{
    if (m_running && m_thread != nullptr)
    {
        m_running = false;
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }
}

void CLoop::WaitEnd()
{
   if (m_running && m_thread != nullptr)
    {
        m_thread->join();
    }
}

/// @brief 
void CLoop::mainLoop()
{
    // Main Loop
    while (m_running)
    {
        m_start = std::chrono::high_resolution_clock::now();
        // Doing some things ...
        m_mutex.lock();
        for (auto Subcriber : m_subscribers)
        {
            if (Subcriber != nullptr)
            {
                Subcriber->OnProcess(m_period);
            }
        }
        m_mutex.unlock();
        m_end = hrc::now();
        // Calculate spent time
        period TimeSpent = std::chrono::duration_cast<std::chrono::microseconds>(m_end-m_start);
        m_lastSleep = m_period-TimeSpent;
        //Sleep the rest of time period
        if (m_lastSleep.count() > 0)
            std::this_thread::sleep_for(m_lastSleep);
    }
}

void CLoop::Subscribe(CProcessEvent* pSubscriber)
{
    if (std::find(m_subscribers.begin(), m_subscribers.end(),pSubscriber) == m_subscribers.end())
    {
        m_mutex.lock();
        m_subscribers.push_back(pSubscriber);
        m_mutex.unlock();
    }
}

void CLoop::UnSubscribe(CProcessEvent* pSubscriber)
{
    m_mutex.lock();
    m_subscribers.erase(std::remove(m_subscribers.begin(), m_subscribers.end(), pSubscriber), m_subscribers.end());
    m_mutex.unlock();
}

period CLoop::GetLastSleep()
{
    return m_lastSleep;
}