// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

/*
We iterate each winddows proceses and check if the it the given process id, if so, we changing the title of the process.
*/
BOOL CALLBACK ChangeProcessTitle(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam
) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    DWORD givenProcessId = *(PDWORD(lParam));
    if(processId == givenProcessId) { //check both IDs
        SetWindowText(hwnd, "This NOTEPAD is PWNED by Gal & Yarden!"); // change notepad title
    }
    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH: {
            DWORD currentProcessId = GetCurrentProcessId(); // get processID
            EnumWindows(ChangeProcessTitle, (LPARAM)&currentProcessId); // iterate window process
        }
    }
    return TRUE;
}

