#pragma once

#include <chrono>
#include <functional>
#include <string>

namespace Debug {
    template<typename duration = std::chrono::milliseconds>
    duration ElapsedTime(std::function<void()> func) {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        func();
        return std::chrono::duration_cast<duration>(std::chrono::steady_clock::now() - start);
    }

    void Log(const char* fmt, ...);
    void Log(const std::string& str);
    void Log(const std::wstring& str);
    void MsgBox(const std::string& text, const std::string& caption = "Debug");
    void MsgBox(const std::wstring& text, const std::wstring& caption = L"Debug");
}