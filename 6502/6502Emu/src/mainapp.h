#pragma once
#include "loop.h"
#include <chrono>

class CMainApp : public CEvent
{
    public:
        CMainApp ();
        ~CMainApp ();

    private:
        hrc::time_point m_timePoint;

    protected:
        void OnUpdate();
};