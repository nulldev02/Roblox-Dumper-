#include <iostream>
#include <cstdlib>

#include "Dumper.h"
#include "Patterns.h"
#include "DiscordClient.h"

int main()
{
    std::cout << "NullDumper 1.1\n\n"; // change the name here 

    DumpResult result;

    auto signatures = GetDefaultSignatures();

    if (!DumpOffsets(
        L"RobloxPlayerBeta.exe",
        signatures,
        result,
        true))
    {
        std::cout << "[-] Dump failed\n";
        system("pause");
        return 1;
    }
    // makes the offsets file and saves the offsets
    if (!SaveOffsetsToHPP("offsets.hpp", result.offsets))
    {
        std::cout << "[-] Failed to save offsets\n";
        system("pause");
        return 1;
    }

    std::cout << "[+] Offsets saved to offsets.hpp\n";

    // this shit sends the webhook the offsets
    std::cout << "[*] Sending webhook...\n";

    if (SendDumpWebhook(result, "offsets.hpp"))
        std::cout << "[+] Webhook sent successfully\n";
    else
        std::cout << "[-] Failed to send webhook\n";

    std::cout << "\nPress any key to exit...\n";
    system("pause");

    return 0;
}