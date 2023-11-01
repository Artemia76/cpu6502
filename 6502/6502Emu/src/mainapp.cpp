#include <iostream>
#include "mainapp.h"

CMainApp::CMainApp()
{
    using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_JSR;
	mem[0xFF01] = 0x00;
	mem[0xFF02] = 0x80;
}

CMainApp::~CMainApp()
{

}

void CMainApp::OnUpdate()
{
    hrc::time_point top = hrc::now();
    if (top > (m_timePoint + std::chrono::seconds(1)))
    {
        m_timePoint = top;
        std::cout << "Top 1 second\n";
    }
}