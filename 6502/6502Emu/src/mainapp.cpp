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

/*****************************************************************************/

CMainApp::CMainApp(CLoop& pParent) : CProcessEvent(pParent), _mem(_bus, 0x0000, 0x0000), _cpu(_bus)
{
#ifdef WIN32
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
    _timePoint = hrc::now();
    using namespace m6502;
	_cpu.reset( 0x4000 );
	/* Progam in assembly
     * Loop and increment X register
	
    * = $4000

	ldx #00
    start
	inx
  	jmp start
	*/
	Byte TestPrg [] = 
		{ 0x00,0x40,0xA2,0x00,0xE8,0x4C,0x02,0x40 };

	_cpu.loadPrg( TestPrg, sizeof(TestPrg) );

    _clock = 3; // Set Clock speed in MHz
}

/*****************************************************************************/

CMainApp::~CMainApp()
{

}

/*****************************************************************************/

void CMainApp::onProcess(const period& pInterval)
{
    using namespace m6502;
    hrc::time_point start = hrc::now();
   std::int64_t Interval = std::chrono::duration_cast<std::chrono::microseconds>(pInterval).count();
    // Compute how much clock cycle to do in interval
    std::int64_t ExpectedCycle = _clock * Interval;
    std::chrono::nanoseconds IDLE_Time = std::chrono::nanoseconds((Interval*1000) / ExpectedCycle);
	std::int64_t ActualCycles = _cpu.execute( ExpectedCycle );
    hrc::time_point end = hrc::now();
    std::int64_t ExecTime = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();


    // Display statistic every second 
    if (start > (_timePoint + std::chrono::seconds(1)))
    {
        _timePoint = start;
        std::cout << "Top 1 second : Calling interval = " << Interval
                  << " µsec , Execution Time = " << ExecTime
                  << " µsec , Cycles Executed = " << ActualCycles
                  << " CPU X = " << std::hex << static_cast<int>(_cpu.X) << std::dec
                  << " CPU Idle Clice Time = " << IDLE_Time.count() << " nsec"
                  << " SleepTime = " << getLastSleep().count() << " µsec"
                  << std::endl;
    }
}