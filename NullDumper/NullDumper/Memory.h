#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace Memory {
    DWORD GetProcessIdByName(const std::wstring& processName);

    HANDLE OpenProcessHandle(DWORD pid);

    void CloseProcessHandle(HANDLE hProcess);

    bool GetModuleInfo(DWORD pid, const std::wstring& moduleName, uintptr_t& baseAddress, SIZE_T& moduleSize);

    bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size);

    class ProcessHandleWrapper {
        HANDLE h;
    public:
        ProcessHandleWrapper(DWORD pid) : h(OpenProcessHandle(pid)) {}
        ~ProcessHandleWrapper() { if (h) CloseProcessHandle(h); }
        HANDLE get() const { return h; }
        operator bool() const { return h != nullptr; }
    };
}