#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include "Dumper.h"
bool SendDumpWebhook(
    const DumpResult& result,
    const std::string& filePath
);