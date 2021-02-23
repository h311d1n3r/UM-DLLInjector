#include <iostream>
#include <windows.h>
#include <string>
#include <libloaderapi.h>
#include <filesystem>
#pragma warning( disable : 6031) //disable getchar() warning

using namespace std;

const string VERSION = "1.0";
const string AUTHOR = "HellDiner";

HANDLE openProcess(int pid) {
    return OpenProcess(PROCESS_ALL_ACCESS, false, pid);
}

LPVOID allocateMemory(HANDLE process, string dll_path) {
    return VirtualAllocEx(process, NULL, dll_path.size() + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void writeDLLPath(HANDLE process, string dll_path, LPVOID addr) {
    WriteProcessMemory(process, addr, dll_path.c_str(), dll_path.size() + 1, NULL);
}

HANDLE loadDLL(HANDLE process, LPVOID addr) {
    return CreateRemoteThread(process, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, addr, NULL, nullptr);
}

bool ends_with(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 11);
    cout << "--- UM-DLLInjector ---" << endl;
    cout << " Version : " << VERSION << endl;
    cout << " Author : " << AUTHOR << endl;
    cout << "----------------------" << endl << endl;
    SetConsoleTextAttribute(hConsole, 13);
    cout << "Input Process ID : ";
    SetConsoleTextAttribute(hConsole, 10);
    int pid;
    cin >> pid;
    cin.ignore();
    SetConsoleTextAttribute(hConsole, 13);
    cout << "Input DLL path : ";
    SetConsoleTextAttribute(hConsole, 10);
    string dll_path;
    getline(cin, dll_path);
    string upperPath = dll_path;
    for (auto& c : upperPath) c = toupper(c);
    if (ends_with(upperPath, ".DLL") && filesystem::exists(dll_path)) {
        HANDLE process = openProcess(pid);
        LPVOID allocation_start_addr = allocateMemory(process, dll_path);
        writeDLLPath(process, dll_path, allocation_start_addr);
        HANDLE thread = loadDLL(process, allocation_start_addr);
        if (!thread) {
            SetConsoleTextAttribute(hConsole, 12);
            cout << "Unable to inject DLL..." << endl;
        }
        SetConsoleTextAttribute(hConsole, 10);
        cout << "DLL successfully injected !" << endl;
    }
    else {
        SetConsoleTextAttribute(hConsole, 12);
        cout << "Path is not correct..." << endl;
    }
    SetConsoleTextAttribute(hConsole, 15);
    getchar();
    return 0;
}