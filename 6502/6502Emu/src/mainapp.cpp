/**
 * @file mainapp.cpp
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

#include <cinttypes>
#include <iostream>
#include <array>
#ifdef WIN32
#include <Windows.h>
#endif
#include "mainapp.hpp"

/**
 * @brief Construct a new CMainApp::CMainApp object
 * 
 */
CMainApp::CMainApp(CLoop& pParent) : CProcessEvent(pParent), cpu(bus), mem(bus, 0x0000, 0x0000)
{
#ifdef WIN32
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
    m_timePoint = hrc::now();
    using namespace m6502;
	cpu.Reset( 0x4000 );
   // when:
	/*
	* = $4000

	ldx #00
    start
	inx
  	jmp start
	*/
	Byte TestPrg [] = 
		{ 0x00,0x40,0xA2,0x00,0xE8,0x4C,0x02,0x40 };

	Word StartAddress = cpu.LoadPrg( TestPrg, sizeof(TestPrg) );
	cpu.PC = StartAddress;

    m_clock = 3; // Set Clock speed in MHz
}

/**
 * @brief Destroy the CMainApp::CMainApp object
 * 
 */
CMainApp::~CMainApp()
{

}

/**
 * @brief 
 * 
 */
void CMainApp::OnProcess(const period& pInterval)
{
    using namespace m6502;
    hrc::time_point start = hrc::now();
   std::int64_t Interval = std::chrono::duration_cast<std::chrono::microseconds>(pInterval).count();
    // Compute how much clock cycle to do in interval
    std::int64_t ExpectedCycle = m_clock * Interval;
    std::chrono::nanoseconds IDLE_Time = std::chrono::nanoseconds((Interval*1000) / ExpectedCycle);
	std::int64_t ActualCycles = cpu.Execute( ExpectedCycle );
    hrc::time_point end = hrc::now();
    std::int64_t ExecTime = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();


    // Display statistic every second 
    if (start > (m_timePoint + std::chrono::seconds(1)))
    {
        m_timePoint = start;
        std::cout << "Top 1 second : Calling interval = " << Interval
                  << " µsec , Execution Time = " << ExecTime
                  << " µsec , Cycles Executed = " << ActualCycles
                  << " CPU X = " << std::hex << static_cast<int>(cpu.X) << std::dec
                  << " CPU Idle Clice Time = " << IDLE_Time.count() << " nsec"
                  << " SleepTime = " << m_parent.GetLastSleep().count() << " µsec"
                  << std::endl;
    }
}