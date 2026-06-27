#include "DiscordClient.h"

#include <windows.h>
#include <winhttp.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#pragma comment(lib,"winhttp.lib")

const wchar_t* WEBHOOK_HOST = L"discord.com";
const wchar_t* WEBHOOK_PATH =
L"/api/webhooks/12321323123123313/cX_FT5iUa1dD_ryl6jN3uF53eGGRUYou2PiIibwpVI_CG7LAs_6"; //replace this shit with your webhook link

const char* DUMPER_VERSION = "V1.1";

bool SendDumpWebhook(
    const DumpResult& result,
    const std::string& filePath)
{
    std::stringstream offsets;

    for (auto& o : result.offsets)
    {
        offsets << o.first
            << " : 0x"
            << std::hex
            << std::uppercase
            << o.second
            << "\\n";
    }

    std::stringstream json;

    json <<
        "{"
        "\"embeds\":[{"
        "\"title\":\"SkidClub Offset Dump\"," // change the title on what the webhook would title the offsets
        "\"color\":3447003,"
        "\"fields\":["
        "{"
        "\"name\":\"Dumper Version\","
        "\"value\":\"V1.1\","  // your dumper version
        "\"inline\":true"
        "},"
        "{"
        "\"name\":\"Roblox Version\","  // shows the roblox version
        "\"value\":\"" << result.robloxVersion << "\","
        "\"inline\":true"
        "},"
        "{"
        "\"name\":\"Dump Time\"," // how long it took 
        "\"value\":\"" << result.dumpTime << "\","
        "\"inline\":true"
        "}"
        "]"
        "}]"
        "}";

    std::string jsonPayload = json.str();

    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        return false;

    std::vector<char> fileData(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    std::string boundary = "----SkidBoundary";

    std::stringstream body;

    body << "--" << boundary << "\r\n";
    body << "Content-Disposition: form-data; name=\"payload_json\"\r\n\r\n";
    body << jsonPayload << "\r\n";

    body << "--" << boundary << "\r\n";
    body << "Content-Disposition: form-data; name=\"file\"; filename=\"offsets.hpp\"\r\n";
    body << "Content-Type: application/octet-stream\r\n\r\n";

    std::string bodyStr = body.str();

    bodyStr.append(fileData.begin(), fileData.end());

    bodyStr += "\r\n--" + boundary + "--\r\n";

    HINTERNET session =
        WinHttpOpen(
            L"NullDumper", 
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

    if (!session)
        return false;

    HINTERNET connect =
        WinHttpConnect(
            session,
            WEBHOOK_HOST,
            INTERNET_DEFAULT_HTTPS_PORT,
            0);

    if (!connect)
    {
        WinHttpCloseHandle(session);
        return false;
    }

    HINTERNET request =
        WinHttpOpenRequest(
            connect,
            L"POST",
            WEBHOOK_PATH,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);

    if (!request)
    {
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return false;
    }

    std::wstring headers =
        L"Content-Type: multipart/form-data; boundary=----SkidBoundary\r\n";

    BOOL sent =
        WinHttpSendRequest(
            request,
            headers.c_str(),
            -1,
            (LPVOID)bodyStr.data(),
            bodyStr.size(),
            bodyStr.size(),
            0);

    if (!sent)
    {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return false;
    }

    BOOL response = WinHttpReceiveResponse(request, NULL);

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);

    return response == TRUE;
}