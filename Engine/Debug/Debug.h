#pragma once

#include <string>

namespace Debug {
    void Log(const std::string& str);
    void Log(const std::wstring& str);
    void MsgBox(const std::string& text, const std::string& caption = "Debug");
    void MsgBox(const std::wstring& text, const std::wstring& caption = L"Debug");
}