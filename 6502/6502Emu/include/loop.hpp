/**
 * @file loop.hpp
 * @author Gianni Peschiutta
 * @brief 6502Emu - Motorola 6502 CPU Emulator
 * @version 0.1
 * @date 2023-11-05
 * 
 * @copyright Copyright (c) 2023
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#ifndef LOOP_HPP
#define LOOP_HPP

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
                CLoop&      m_parent;
    protected:
    virtual void OnProcess(const period& pInterval)=0;
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
    period          GetLastSleep ();

    private:
    std::thread*    m_thread;
    hrc::time_point m_start;
    hrc::time_point m_end;
    period          m_period;
    period          m_lastSleep;
    std::atomic<bool> m_running;
    v_subscribers   m_subscribers;
    std::mutex      m_mutex;
    void            mainLoop();
};

#endif