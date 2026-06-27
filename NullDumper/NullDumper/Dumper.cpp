#define NOMINMAX
#include <Windows.h>

#include "Dumper.h"
#include "Memory.h"
#include "Patterns.h"



#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

constexpr SIZE_T CHUNK_SIZE = 0x200000; 

bool IsExecutable(DWORD protect)
{
    return protect & (
        PAGE_EXECUTE |
        PAGE_EXECUTE_READ |
        PAGE_EXECUTE_READWRITE
        );
}

std::string GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    tm local{};
    localtime_s(&local, &t);

    std::stringstream ss;
    ss << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string GetRobloxVersion()
{

    char* localAppData = nullptr;
    size_t len = 0;

    _dupenv_s(&localAppData, &len, "LOCALAPPDATA");

    if (!localAppData)
        return "Unknown";

    std::filesystem::path path =
        std::filesystem::path(localAppData) /
        "Fishstrap" /
        "Versions";

    free(localAppData);

    if (!std::filesystem::exists(path))
        return "Unknown";

    std::string newest;
    std::filesystem::file_time_type newestTime{};

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (!entry.is_directory())
            continue;

        std::string name = entry.path().filename().string();

        if (name.rfind("version-", 0) == 0)
        {
            auto time = std::filesystem::last_write_time(entry);

            if (newest.empty() || time > newestTime)
            {
                newest = name;
                newestTime = time;
            }
        }
    }

    return newest.empty() ? "Unknown" : newest;
}

bool DumpOffsets(
    const std::wstring& processName,
    const std::vector<Signature>& signatures,
    DumpResult& result,
    bool verbose)
{
    DWORD pid = Memory::GetProcessIdByName(processName);
    if (!pid)
    {
        std::cout << "[-] Process not found\n";
        return false;
    }

    Memory::ProcessHandleWrapper proc(pid);
    if (!proc)
    {
        std::cout << "[-] Failed to open process\n";
        return false;
    }

    uintptr_t base = 0;
    SIZE_T size = 0;

    if (!Memory::GetModuleInfo(pid, processName, base, size))
    {
        std::cout << "[-] Failed module info\n";
        return false;
    }

    uintptr_t end = base + size;

    std::cout << "[+] Base: 0x" << std::hex << base << "\n";
    std::cout << "[+] Size: 0x" << size << std::dec << "\n\n";

    std::vector<std::pair<std::string, uintptr_t>> results;

    MEMORY_BASIC_INFORMATION mbi{};
    uintptr_t addr = base;

    while (addr < end)
    {
        if (!VirtualQueryEx(proc.get(), (LPCVOID)addr, &mbi, sizeof(mbi)))
            break;

        uintptr_t regionBase = (uintptr_t)mbi.BaseAddress;
        uintptr_t regionEnd = regionBase + mbi.RegionSize;

        addr = regionEnd;

        if (mbi.State != MEM_COMMIT || !IsExecutable(mbi.Protect))
            continue;

        SIZE_T regionSize = mbi.RegionSize;
        if (regionBase + regionSize > end)
            regionSize = end - regionBase;

        if (verbose)
        {
            std::cout << "[+] Scanning region: 0x"
                << std::hex << regionBase << " -> 0x" << regionEnd << std::dec << "\n";
        }

        uintptr_t chunk = regionBase;

        while (chunk < regionBase + regionSize)
        {
            SIZE_T readSize = std::min(CHUNK_SIZE, (regionBase + regionSize) - chunk);

            std::vector<BYTE> buffer(readSize);
            SIZE_T bytesRead = 0;

            if (!ReadProcessMemory(proc.get(), (LPCVOID)chunk, buffer.data(), readSize, &bytesRead))
            {
                chunk += readSize;
                continue;
            }

            for (const auto& sig : signatures)
            {
                if (std::find_if(results.begin(), results.end(),
                    [&](auto& r) { return r.first == sig.name; }) != results.end())
                    continue;

                std::vector<BYTE> pattern;
                std::string mask;

                if (!PatternToBytes(sig.pattern, pattern, mask))
                    continue;

                if (bytesRead < pattern.size())
                    continue;

                for (size_t i = 0; i <= bytesRead - pattern.size(); i++)
                {
                    bool match = true;

                    for (size_t j = 0; j < pattern.size(); j++)
                    {
                        if (mask[j] == 'x' && buffer[i + j] != pattern[j])
                        {
                            match = false;
                            break;
                        }
                    }

                    if (!match)
                        continue;

                    uintptr_t instr = chunk + i;
                    uintptr_t finalAddr = instr;

            
                    if (sig.ripOffset != -1)
                    {
                        if (i + sig.ripOffset + 4 > bytesRead)
                            continue;

                        int32_t disp;
                        memcpy(&disp, &buffer[i + sig.ripOffset], 4);

                        finalAddr = instr + sig.instructionLength + disp;
                    }

                    if (finalAddr < base || finalAddr > end)
                        continue;

                    uintptr_t offset = finalAddr - base;

                    std::cout << "[+] "
                        << sig.name
                        << " = 0x"
                        << std::hex << offset << std::dec << "\n";

                    results.push_back({ sig.name, offset });
                    break;
                }
            }

            chunk += readSize;
        }
    }

    if (results.empty())
    {
        std::cout << "[-] No offsets found\n";
        return false;
    }

    result.offsets = results;
    result.dumpTime = GetCurrentTimeString();
    result.robloxVersion = GetRobloxVersion();

    SaveOffsetsToHPP("offsets.hpp", results);

    std::cout << "\n[+] Dump complete\n";

    return true;
}

bool SaveOffsetsToHPP(  // change here what the .hpp file would say (change the name)
    const std::string& filename,
    const std::vector<std::pair<std::string, uintptr_t>>& offsets)
{
    std::ofstream file(filename);
    if (!file) return false;

    file << "#pragma once\n\n";
    file << "#include <Windows.h>\n\n";

    file << "// NullDumper V1.1\n\n"; // here
    file << "#define REBASE(x) (x + (uintptr_t)GetModuleHandle(nullptr))\n\n";

    file << "namespace Offsets {\n\n";

    for (const auto& o : offsets)
    {
        file << "    const uintptr_t "
            << o.first
            << " = REBASE(0x"
            << std::hex << std::uppercase << o.second
            << std::dec << ");\n";
    }

    file << "\n}\n";

    return true;
}