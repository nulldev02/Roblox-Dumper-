#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include "Patterns.h"

struct DumpResult
{
    std::vector<std::pair<std::string, uintptr_t>> offsets;
    std::string robloxVersion;
    std::string dumpTime;
};

bool DumpOffsets(
    const std::wstring& processName,
    const std::vector<Signature>& signatures,
    DumpResult& result,
    bool verbose = false
);

bool SaveOffsetsToHPP(
    const std::string& file,
    const std::vector<std::pair<std::string, uintptr_t>>& offsets
);

std::string GetRobloxVersion();
std::string GetCurrentTimeString();

bool SendDumpToBot(const DumpResult& result, const std::string& filePath);