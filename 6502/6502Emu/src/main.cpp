#include <iostream>
#include "loop.h"
#include "mainapp.h"

/// @brief 
/// @return 
int main() {
    CLoop Loop;
    CMainApp MainApp;
    Loop.Subscribe (&MainApp);
    Loop.Start(200);
    std::cout << "Wait touch press..." << std::endl;
    std::getchar();
    Loop.Stop();
    return 0;
}