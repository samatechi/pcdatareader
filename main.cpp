#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cinttypes>
#include <cstdio>
#pragma comment(lib, "user32.lib")

using namespace std;

class HardwareInformation {
private:
    DWORD pageSize, numberOfProcessors, processorType, allocationGranularity, OemId;
    WORD processorLevel, processorRevision;
    LPVOID minimumApplicationAddress, maximumApplicationAddress;
    DWORD_PTR activeProcessorMask;

public:
    HardwareInformation(
        DWORD dwOemId,
        DWORD dwPageSize,
        LPVOID lpMinimumApplicationAddress,
        LPVOID lpMaximumApplicationAddress,
        DWORD_PTR dwActiveProcessorMask,
        DWORD dwNumberOfProcessors,
        DWORD dwProcessorType,
        DWORD dwAllocationGranularity,
        WORD wProcessorLevel,
        WORD wProcessorRevision) {
        OemId = dwOemId;
        pageSize = dwPageSize;
        numberOfProcessors = dwNumberOfProcessors;
        processorType = dwProcessorType;
        processorLevel = wProcessorLevel;
        processorRevision = wProcessorRevision;
        minimumApplicationAddress = lpMinimumApplicationAddress;
        maximumApplicationAddress = lpMaximumApplicationAddress;
        activeProcessorMask = dwActiveProcessorMask;
        allocationGranularity = dwAllocationGranularity;
    }

    ~HardwareInformation() { cout << "Information deleted." << endl; }

    DWORD get_OemId() { return OemId; }
    DWORD get_PageSize() { return pageSize; }
    LPVOID get_MinimumApplicationAddress() { return minimumApplicationAddress; }
    LPVOID get_MaximumApplicationAddress() { return maximumApplicationAddress; }
    DWORD_PTR get_ActiveProcessorMask() { return activeProcessorMask; }
    DWORD get_NumberOfProcessors() { return numberOfProcessors; }
    DWORD get_ProcessorType() { return processorType; }
    DWORD get_AllocationGranularity() { return allocationGranularity; }
    WORD get_ProcessorLevel() { return processorLevel; }
    WORD get_ProcessorRevision() {return processorRevision; }
};

class MemoryInformation {
    private:
    DWORD length, memoryLoad;
    DWORDLONG totalPhys, availPhys, totalPageFile, availPageFile, totalVirtual, availVirtual, availExtendedVirtual;

    public:
    MemoryInformation(
        DWORD dwLength,
        DWORD dwMemoryLoad,
        DWORDLONG ullTotalPhys,
        DWORDLONG ullAvailPhys,
        DWORDLONG ullTotalPageFile,
        DWORDLONG ullAvailPageFile,
        DWORDLONG ullTotalVirtual,
        DWORDLONG ullAvailVirtual,
        DWORDLONG ullAvailExtendedVirtual) {
        length = dwLength;
        memoryLoad = dwMemoryLoad;
        totalPhys = ullTotalPhys;
        availPhys = ullAvailPhys;
        totalPageFile = ullTotalPageFile;
        availPageFile = ullAvailPageFile;
        totalVirtual = ullTotalVirtual;
        availVirtual = ullAvailVirtual;
        availExtendedVirtual = ullAvailExtendedVirtual;
    }

    ~MemoryInformation() { cout << "Information deleted." << endl; }

    DWORD get_Length() { return length; }
    DWORD get_MemoryLoad() { return memoryLoad; }
    DWORDLONG get_TotalPhys() { return totalPhys; }
    DWORDLONG get_AvailPhys() { return availPhys; }
    DWORDLONG get_TotalPageFile() { return totalPageFile; }
    DWORDLONG get_AvailPageFile() { return availPageFile; }
    DWORDLONG get_TotalVirtual() { return totalVirtual; }
    DWORDLONG get_AvailVirtual() { return availVirtual; }
    DWORDLONG get_AvailExtendedVirtual() { return availExtendedVirtual; }

    void set_MemoryLoad(DWORD load) { memoryLoad = load; }
    void set_AvailPhys(DWORDLONG avail) { availPhys = avail; }
    void set_AvailPageFile(DWORDLONG avail) { availPageFile = avail; }
    void set_AvailVirtual(DWORDLONG avail) { availVirtual = avail; }
};

double GetCPUUsage() {
    static ULONGLONG prevTotalTime = 0;
    static ULONGLONG prevIdleTime = 0;

    ULONGLONG totalTime = 0;
    ULONGLONG idleTime = 0;
    ULONGLONG kernelTime = 0;
    ULONGLONG userTime = 0;

    if (GetSystemTimes(reinterpret_cast<FILETIME*>(&idleTime), reinterpret_cast<FILETIME*>(&kernelTime), reinterpret_cast<FILETIME*>(&userTime))) {
        totalTime = kernelTime + userTime;
        idleTime = idleTime;

        ULONGLONG totalDeltaTime = totalTime - prevTotalTime;
        ULONGLONG idleDeltaTime = idleTime - prevIdleTime;

        if (totalDeltaTime > 0) {
            double cpuUsage = (1.0 - static_cast<double>(idleDeltaTime) / totalDeltaTime) * 100.0;
            prevTotalTime = totalTime;
            prevIdleTime = idleTime;
            return cpuUsage;
        }
    } else {
        cout << "Failed to retrieve CPU usage." << endl;
    }

    return 0.0;
}

void gotoxy(int column, int line) { COORD coord; coord.X = column; coord.Y = line;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main() {
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);

    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    HardwareInformation thisPCHardware(siSysInfo.dwOemId, siSysInfo.dwPageSize, siSysInfo.lpMinimumApplicationAddress, siSysInfo.lpMaximumApplicationAddress, siSysInfo.dwActiveProcessorMask, siSysInfo.dwNumberOfProcessors, siSysInfo.dwProcessorType, siSysInfo.dwAllocationGranularity, siSysInfo.wProcessorLevel, siSysInfo.wProcessorRevision);
    MemoryInformation thisPCMemory(memoryStatus.dwLength, memoryStatus.dwMemoryLoad, memoryStatus.ullTotalPhys, memoryStatus.ullAvailPhys, memoryStatus.ullTotalPageFile, memoryStatus.ullAvailPageFile, memoryStatus.ullTotalVirtual, memoryStatus.ullAvailVirtual, memoryStatus.ullAvailExtendedVirtual);

    while (true) {
        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (GlobalMemoryStatusEx(&memoryStatus)) {
            thisPCMemory.set_MemoryLoad(memoryStatus.dwMemoryLoad);
            thisPCMemory.set_AvailPhys(memoryStatus.ullAvailPhys);
            thisPCMemory.set_AvailPageFile(memoryStatus.ullAvailPageFile);
            thisPCMemory.set_AvailVirtual(memoryStatus.ullAvailVirtual);
        } else {
            cout << "Failed to retrieve memory status." << endl;
        }

        printf("\rHardware information: \n");
        printf("\r    OEM ID: %u\n", thisPCHardware.get_OemId());
        printf("\r    Page size: %u\n", thisPCHardware.get_PageSize());
        printf("\r    Number of processors: %u\n", thisPCHardware.get_NumberOfProcessors());
        printf("\r    Processor type: %u\n", thisPCHardware.get_ProcessorType());
        printf("\r    Processor revision: %u\n", thisPCHardware.get_ProcessorRevision());
        printf("\r    Processor level: %u\n", thisPCHardware.get_ProcessorLevel());
        printf("\r    Active processor mask: %u\n", thisPCHardware.get_ActiveProcessorMask());
        printf("\r    Minimum application address: %lx\n", thisPCHardware.get_MinimumApplicationAddress());
        printf("\r    Maximum application address: %lx\n", thisPCHardware.get_MaximumApplicationAddress());
        printf("\r    Allocation granularity: %u\n", thisPCHardware.get_AllocationGranularity());
        printf("\n");
        printf("\rMemory information: \n");
        printf("\r    Memory length: %u\n", thisPCMemory.get_Length());
        printf("\r    Memory load: %u%\n", thisPCMemory.get_MemoryLoad());
        printf("\r    Total physical memory: %u\n", thisPCMemory.get_TotalPhys());
        printf("\r    Available physical memory: %u\n", thisPCMemory.get_AvailPhys());
        printf("\r    Total page file: %u\n", thisPCMemory.get_TotalPageFile());
        printf("\r    Available page file: %u\n", thisPCMemory.get_AvailPageFile());
        printf("\r    Total virtual memory: %u\n", thisPCMemory.get_TotalVirtual());
        printf("\r    Available virtual memory: %u\n", thisPCMemory.get_AvailVirtual());
        printf("\r    Available extended virtual memory (always 0): %u\n", thisPCMemory.get_AvailExtendedVirtual());
        printf("\n");
        printf("\rCPU information: \n");
        printf("\r    CPU usage: %f%              \n", GetCPUUsage());

        Sleep(1000);
        gotoxy(0,0);
    }
}