/**
 * @file mainapp.hpp
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

#ifndef MAINAPP_HPP
#define MAINAPP_HPP

#include "loop.hpp"
#include <chrono>
#include <m6502/System.hpp>

/**
 * @brief Main 6502 Emulator application
 * 
 */
class CMainApp : public CProcessEvent
{
public:
    /**
     * @brief Construct a new CMainApp object
     * 
     * @param pParent 
     */
    CMainApp (CLoop& pParent);

    /**
     * @brief Destroy the CMainApp object
     * 
     */
    ~CMainApp ();

protected:
    /**
     * @brief Process Event loop callback
     * 
     * @param pInterval : Period of loop
     */
    void onProcess(const period& pInterval);

private:
    /**
     * @brief 
     * 
     */
    hrc::time_point _timePoint;

    /**
     * @brief Data Bus Emulator for CPU and memory
     * 
     */
    m6502::CBus _bus;

    /**
     * @brief Memory to use with CPU
     * 
     */
    m6502::CMem _mem;

    /**
     * @brief CPU emulator
     * 
     */
    m6502::CCPU _cpu;

    /**
     * @brief Clock in Mhz
     * 
     */
    int _clock;
};

#endif