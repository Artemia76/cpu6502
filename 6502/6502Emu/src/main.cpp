#include <iostream>
#include "m6502.h"
#include "loop.h"
#include "mainapp.h"

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