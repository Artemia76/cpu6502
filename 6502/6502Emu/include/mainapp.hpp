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

class CMainApp : public CProcessEvent
{
    public:
        CMainApp (CLoop& pParent);
        ~CMainApp ();

    private:
        hrc::time_point m_timePoint;
        m6502::Mem      mem;
        m6502::CPU      cpu;
        int             m_clock; // Clock in Mhz

    protected:
        void OnProcess(const period& pInterval);
};

#endif