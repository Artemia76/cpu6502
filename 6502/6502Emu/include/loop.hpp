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
friend class CLoop;
public:
    /**
     * @brief Construct a new CProcessEvent object
     * 
     * @param pParent 
     */
    CProcessEvent(CLoop& pParent);

    /**
     * @brief Destroy the CProcessEvent object
     * 
     */
    ~CProcessEvent();

protected:

    /**
     * @brief Process Event Callback
     * 
     * @param pInterval 
     */
virtual void onProcess(const period& pInterval)=0;

    /**
     * @brief 
     * 
     */
    period getLastSleep();

    private:
    /**
     * @brief Store event controller
     * 
     */
    CLoop&  _parent;
};

/**
 * @brief Container for event clients
 * 
 */
typedef std::vector<CProcessEvent*> v_subscribers;

/**
 * @brief Loop Class Ensure Main loop management 
 * 
 * 
 */
class CLoop
{
friend class CProcessEvent;
public:
    /**
     * @brief Construct a new CLoop object
     * 
     */
    CLoop ();

    /**
     * @brief Destroy the CLoop object
     * 
     */
    ~CLoop ();

    /**
     * @brief Start the loop processing with given period
     * 
     * @param pPeriod : Period in msec
     */
    void start(int pPeriod=10);

    /**
     * @brief Stop the loop processing
     * 
     */
    void stop();

    period getLastSleep ();

protected:
    /**
     * @brief Suscribe to event loop
     * 
     * @param pSubscriber 
     */
    void subscribe (CProcessEvent* pSubscriber);

    /**
     * @brief Unsuscribe to event loop
     * 
     * @param pSubscriber 
     */
    void unSubscribe (CProcessEvent* pSubscriber);

private:
    /**
     * @brief Thread for loop
     * 
     */
    std::thread*    _thread;

    /**
     * @brief Start Loop Time
     * 
     */
    hrc::time_point _start;

    /**
     * @brief End Loop time
     * 
     */
    hrc::time_point _end;

    /**
     * @brief Loop Period Execution
     * 
     */
    period          _period;

    /**
     * @brief Time sleeping on last loop (Free Time)
     * 
     */
    period          _lastSleep;

    /**
     * @brief State of current loop Execution
     * 
     */
    std::atomic<bool> _running;

    /**
     * @brief Suscribers container for loop event
     * 
     */
    v_subscribers   _subscribers;

    /**
     * @brief 
     * 
     */
    std::mutex      _mutex;

    void            _mainLoop();
};

#endif