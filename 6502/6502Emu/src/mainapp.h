#pragma once
#include "loop.h"
#include <chrono>
#include "m6502.h"

class CMainApp : public CEvent
{
    public:
        CMainApp ();
        ~CMainApp ();

    private:
        hrc::time_point m_timePoint;
	    m6502::Mem mem;
	    m6502::CPU cpu;

    protected:
        void OnUpdate();
};