/**
 * @file loop.cpp
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

#include "loop.hpp"
#include <algorithm>

CProcessEvent::CProcessEvent(CLoop& pParent) : _parent(pParent)
{
    _parent.subscribe(this);
}

/*****************************************************************************/

CProcessEvent::~CProcessEvent()
{
    _parent.unSubscribe(this);
}

/*****************************************************************************/

period CProcessEvent::getLastSleep()
{
    return _parent.getLastSleep();
}

/*****************************************************************************/

CLoop::CLoop ()
{
    _running=false;
    _thread = nullptr;
    _lastSleep = std::chrono::milliseconds(0);
}

/*****************************************************************************/

CLoop::~CLoop ()
{
    if (_running) stop();
}

/*****************************************************************************/

void CLoop::start(int pPeriod)
{
    _period = std::chrono::microseconds(pPeriod*1000);
    if ((!_running) && (_thread==nullptr))
    {
        _running = true;
        _thread= new std::thread(&CLoop::_mainLoop, this);
    }
}

/*****************************************************************************/

void CLoop::stop()
{
    if (_running && _thread != nullptr)
    {
        _running = false;
        _thread->join();
        delete _thread;
        _thread = nullptr;
    }
}

/*****************************************************************************/

void CLoop::_mainLoop()
{
    // Main Loop
    while (_running)
    {
        _start = std::chrono::high_resolution_clock::now();
        // Doing some things ...
        _mutex.lock();
        for (auto Subcriber : _subscribers)
        {
            if (Subcriber != nullptr)
            {
                Subcriber->onProcess(_period);
            }
        }
        _mutex.unlock();
        _end = hrc::now();
        // Calculate spent time
        period TimeSpent = std::chrono::duration_cast<std::chrono::microseconds>(_end-_start);
        _lastSleep = _period-TimeSpent;
        //Sleep the rest of time period
        if (_lastSleep.count() > 0)
            std::this_thread::sleep_for(_lastSleep);
    }
}

/*****************************************************************************/

void CLoop::subscribe(CProcessEvent* pSubscriber)
{
    if (std::find(_subscribers.begin(), _subscribers.end(),pSubscriber) == _subscribers.end())
    {
        _mutex.lock();
        _subscribers.push_back(pSubscriber);
        _mutex.unlock();
    }
}

/*****************************************************************************/

void CLoop::unSubscribe(CProcessEvent* pSubscriber)
{
    _mutex.lock();
    _subscribers.erase(std::remove(_subscribers.begin(), _subscribers.end(), pSubscriber), _subscribers.end());
    _mutex.unlock();
}

/*****************************************************************************/

period CLoop::getLastSleep()
{
    return _lastSleep;
}