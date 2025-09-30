// main.cpp 

#include <fstream>
#include <Windows.h>

#include "virtualmachine.h"

LPWSTR* getCommandLineArguments();
std::vector<uint8_t> getRomData(LPWSTR romName);

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    LPWSTR* argv = getCommandLineArguments();
    if (argv == nullptr)
        return 1;

    std::vector<uint8_t> romData = getRomData(argv[1]);
    LocalFree(argv);
    if (romData.empty())
        return 1;

    VirtualMachine vm;
    vm.Execute(romData.data(), romData.size());
    return 0;
}

LPWSTR* getCommandLineArguments()
{
    LPWSTR lpCmdLineW = GetCommandLineW();

    int argc;
    LPWSTR* argv = CommandLineToArgvW(lpCmdLineW, &argc);

    if (argc < 2) {
        LocalFree(argv);
        return nullptr;
    }

    return argv;
}

std::vector<uint8_t> getRomData(LPWSTR romName)
{
    std::ifstream rom(romName, std::ios::binary | std::ios::ate);
    if (!rom.is_open()) {
        return {};
    }

    std::streamsize size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    std::vector<uint8_t> romData(size);
    if (!rom.read(reinterpret_cast<char*>(romData.data()), size)) {
        return {};
    }

    rom.close();

    return romData;
}