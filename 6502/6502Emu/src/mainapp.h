#pragma once
#include "loop.h"
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