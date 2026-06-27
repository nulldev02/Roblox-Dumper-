#pragma once
#include <windows.h>
#include <string>
#include <vector>

// structure
struct Signature {
    std::string name;
    std::string pattern;
    int ripOffset;
    int instructionLength;
};

bool PatternToBytes(const std::string& pattern, std::vector<BYTE>& outBytes, std::string& outMask);

uintptr_t FindPatternInBuffer(const BYTE* buffer, SIZE_T bufferSize, const std::vector<BYTE>& pattern, const std::string& mask);

uintptr_t ResolveRIPAddress(uintptr_t patternAddress, int32_t displacement, int instructionLength);

std::vector<Signature> GetDefaultSignatures();