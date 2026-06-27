#include "Patterns.h"
#include <vector>
#include <string>
#include <cstdlib>

bool PatternToBytes(const std::string& pattern, std::vector<BYTE>& outBytes, std::string& outMask)
{
    outBytes.clear();
    outMask.clear();

    for (size_t i = 0; i < pattern.length();)
    {
        if (pattern[i] == ' ')
        {
            i++;
            continue;
        }

        if (pattern[i] == '?')
        {
            outBytes.push_back(0);
            outMask += '?';

            if (i + 1 < pattern.length() && pattern[i + 1] == '?')
                i += 2;
            else
                i++;

            continue;
        }

        if (i + 1 >= pattern.length())
            return false;

        char byteStr[3] = { pattern[i], pattern[i + 1], 0 };

        BYTE byte = static_cast<BYTE>(strtoul(byteStr, nullptr, 16));

        outBytes.push_back(byte);
        outMask += 'x';

        i += 2;
    }

    return !outBytes.empty();
}

uintptr_t FindPatternInBuffer(
    const BYTE* buffer,
    SIZE_T bufferSize,
    const std::vector<BYTE>& pattern,
    const std::string& mask)
{
    const size_t patternLen = pattern.size();

    if (patternLen == 0 || patternLen > bufferSize)
        return SIZE_MAX;

    for (size_t i = 0; i <= bufferSize - patternLen; i++)
    {
        bool found = true;

        for (size_t j = 0; j < patternLen; j++)
        {
            if (mask[j] == 'x' && buffer[i + j] != pattern[j])
            {
                found = false;
                break;
            }
        }

        if (found)
            return i;
    }

    return SIZE_MAX;
}

uintptr_t ResolveRIPAddress(uintptr_t instructionAddress, int32_t displacement, int instructionLength)
{
    return instructionAddress + instructionLength + displacement;
}

// the patterns
std::vector<Signature> GetDefaultSignatures()
{
    std::vector<Signature> sigs;

    sigs.push_back({
        "luaD_throw",
        "48 83 EC ? 44 8B C2 48 8B D1 48 8D 4C 24",
        -1,
        0
        });

    sigs.push_back({
        "ScriptContextResume",
        "48 8B C4 44 89 48 20 4C 89 40 18 48 89 50 10 48 89 48 08 53",
        -1,
        0
        });

    sigs.push_back({
        "OpcodeLookupTable",
        "4C 38 02 8E FC 34 0C 70 00 1B A2 28 6C 6A 62 42 22 16 2A 53 0B 46 03 B0 C2 BC 36 7B 7C 63 32 90 20 3E 84 27 56 3B 58 DE BE 3C 7A 68 13 3F 50 5E CC 9C 66 D2 E2 8A B6 51 61 44 3A 52 23 4A 5F E1",
        -1,
        0
        });

    sigs.push_back({
    "Print",
    "E8 ? ? ? ? 83 FB ? 75 ? 4C 8B 44 24",
    1,
    5
        });

    sigs.push_back({
    "Task_Defer",
    "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? 48 8B F9 E8",
    -1,
    0
        });

    sigs.push_back({
     "Task_Spawn",
     "48 89 5C 24 ? 55 56 57 48 83 EC ? 48 8B D9 E8 ? ? ? ?",
     -1,
     0
        });
    sigs.push_back({
        "PushInstance",
        "48 89 5C 24 08 57 48 83 EC ? 48 8B FA 48 8B D9 E8 ? ? ? ? 84 C0 74 ?",
        -1,
        0
        });

    sigs.push_back({
        "PushInstance2",
        "E8 ? ? ? ? 48 8B D8 48 85 C0 74 ?",
        1,
        5
        });

    sigs.push_back({
        "RawScheduler",
        "48 89 05 ? ? ? ? 48 8D 3D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ?",
        -1,
        0
        });

    sigs.push_back({
        "LuaO_nilobject",
        "48 8D 3D ? ? ? ? 48 3B D7",
        3,
        7
        });

    sigs.push_back({
    "Luau_Execute",
    "80 79 06 00 0F 85 ? ? ? ? E9 ? ? ? ?",
    -1,
    0
        });

    sigs.push_back({
    "LuaH_Dummynode",
    "48 8d 3d ? ? ? ? 48 8B D9 48 39",
    3,
    7
        });

    sigs.push_back({
    "Impersonator",
    "48 89 5C 24 18 48 89 6C 24 20 56 57 41 56 48 83 EC 30 48 8B 01 49 8B F9 49 8B E8 48 89 54 24 58 48 8B F2 4C 8B F1",
    -1,
    0
        });

    sigs.push_back({
        "KTable",
        "48 8D 0D ? ? ? ? 48 8D 54 24 ? 48 8B 04 C1",
        3,
        7
        });

    sigs.push_back({
        "FireRightMouseClick",
        "48 89 5C 24 ? 55 56 57 48 83 EC ? 49 8B F8 48 8B F1 33 ED 89 AC 24 ? ? ? ? F3 0F 10 81 ? ? ? ? 0F 2F C1",
        -1,
        0
        });

    sigs.push_back({
        "FireMouseClick",
        "E8 ? ? ? ? 48 89 5C 24 18 55 56 57 48 83 EC 70",
        1,
        5
        });

    sigs.push_back({
        "FireMouseHoverEnter",
        "E8 ? ? ? ? 48 89 5C 24 08 48 89 6C 24 18 48 89 74 24 20",
        1,
        5
        });

    sigs.push_back({
        "TaskScheduler",
        "48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 88",
        3,
        7
        });

    sigs.push_back({
        "FakeDataModel",
        "48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01",
        3,
        7
        });

    


    return sigs;
}