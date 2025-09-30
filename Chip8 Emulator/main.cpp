// main.cpp 

#include <fstream>
#include <string>
#include <Windows.h>

#include "virtualmachine.h"

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    std::ifstream pongFile("ibm.ch8", std::ios::binary | std::ios::ate);
    if (!pongFile.is_open()) {
        return 1;
    }
    std::streamsize size = pongFile.tellg();

    pongFile.seekg(0, std::ios::beg);

    std::vector<uint8_t> pongRom(size);
    if (!pongFile.read(reinterpret_cast<char*>(pongRom.data()), size)) {
        return 1;
    }

    pongFile.close();

    VirtualMachine vm;
    vm.Execute(pongRom.data(), pongRom.size());

    return 0;
}