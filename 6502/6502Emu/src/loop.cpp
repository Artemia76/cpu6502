#include "loop.h"

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
                Subcriber->OnUpdate();
            }
        }
        m_mutex.unlock();
        m_end = hrc::now();
        // Calculate spent time
        period TimeSpent = std::chrono::duration_cast<std::chrono::microseconds>(m_end-m_start);
        //Sleep the rest of time period
        std::this_thread::sleep_for(m_period-TimeSpent);
    }
}

void CLoop::Subscribe(CEvent* pSubscriber)
{
    if (std::find(m_subscribers.begin(), m_subscribers.end(),pSubscriber) == m_subscribers.end())
    {
        m_mutex.lock();
        m_subscribers.push_back(pSubscriber);
        m_mutex.unlock();
    }
}

void CLoop::UnSubscribe(CEvent* pSubscriber)
{
    for (v_subscribers::iterator it ; it != m_subscribers.end(); ++it)
    {
        if (pSubscriber == (*it))
        {
            m_mutex.lock();
            m_subscribers.erase(it);
            m_mutex.unlock();
        }
    }

}