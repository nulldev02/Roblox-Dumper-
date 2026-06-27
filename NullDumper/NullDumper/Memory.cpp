#include "Memory.h"
#include <tlhelp32.h>
#include <iostream>

namespace Memory {

    DWORD GetProcessIdByName(const std::wstring& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
        DWORD pid = 0;

        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                    pid = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return pid;
    }

    HANDLE OpenProcessHandle(DWORD pid) {
        return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    }

    void CloseProcessHandle(HANDLE hProcess) {
        if (hProcess) CloseHandle(hProcess);
    }

    bool GetModuleInfo(DWORD pid, const std::wstring& moduleName, uintptr_t& baseAddress, SIZE_T& moduleSize) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snapshot == INVALID_HANDLE_VALUE) return false;

        MODULEENTRY32W entry = { sizeof(MODULEENTRY32W) };
        bool found = false;

        if (Module32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szModule, moduleName.c_str()) == 0) {
                    baseAddress = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
                    moduleSize = entry.modBaseSize;
                    found = true;
                    break;
                }
            } while (Module32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return found;
    }

    bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size) {
        SIZE_T bytesRead = 0;
        return ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), buffer, size, &bytesRead) && bytesRead == size;
    }

}