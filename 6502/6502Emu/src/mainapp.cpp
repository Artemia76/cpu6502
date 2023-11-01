#include <iostream>
#include "mainapp.h"

CMainApp::CMainApp()
{
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