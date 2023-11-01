#include <cinttypes>
#include <iostream>
#include "mainapp.h"

/**
 * @brief Construct a new CMainApp::CMainApp object
 * 
 */
CMainApp::CMainApp(CLoop& pParent) : CProcessEvent(pParent)
{
    m_timePoint = hrc::now();
    using namespace m6502;
	cpu.Reset( 0x4000, mem );
   // when:
	/*
	* = $4000

	ldx #00
    start
	inx
  	jmp start
	*/
	Byte TestPrg[] = 
		{ 0x00,0x40,0xA2,0x00,0xE8,0x4C,0x02,0x40 };

	Word StartAddress = cpu.LoadPrg( TestPrg, sizeof(TestPrg), mem );
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
    int Interval = std::chrono::duration_cast<std::chrono::milliseconds>(pInterval).count();
    // Compute how much clock cycle to do in interval
    int ExpectedCycle = m_clock * 1000 * Interval;

	int ActualCycles = cpu.Execute( ExpectedCycle, mem );
    hrc::time_point end = hrc::now();
    int ExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();


    // Display statistic every second 
    if (start > (m_timePoint + std::chrono::seconds(1)))
    {
        m_timePoint = start;
        std::cout << "Top 1 second : Calling interval = "
            << Interval << " msec , Execution Time = " 
            << ExecTime << " msec , Cycles Executed = "
            << ActualCycles << " CPU X = " << std::hex << static_cast<int>(cpu.X) << std::dec
            << std::endl;
    }
}