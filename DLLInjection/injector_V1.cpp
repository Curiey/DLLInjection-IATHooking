// injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

int main(int argc, char** argv)
{   
    if (argc == 2) {
        // LoadLibrary sectrrion
        HMODULE kernelM = GetModuleHandle("Kernel.dll");
        FARPROC loadLibraryAdress = GetProcAddress(kernelM, "LoadLibrary");

        // Allocate Memory section
        HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, atoi(argv[1]));
        LPVOID allocMem = VirtualAllocEx(handle, NULL, sizeof(*"injected.dll") + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        WriteProcessMemory(handle, allocMem, "injected.dll", sizeof(*"injected.dll") + 1,NULL);
        
        // inject .dll file
        CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAdress, allocMem, 0, NULL);
        std::cout << "Hello World!\n";
    }
}