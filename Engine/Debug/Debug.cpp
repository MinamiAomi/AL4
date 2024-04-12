#include "Debug.h"

#include <Windows.h>

namespace Debug {
    void Log(const std::string& str) {
        OutputDebugStringA(str.c_str());
    }
    void Log(const std::wstring& str) {
        OutputDebugStringW(str.c_str());
    }
    void MsgBox(const std::string& text, const std::string& caption) {
        MessageBoxA(nullptr, text.c_str(), caption.c_str(), S_OK);
    }
    void MsgBox(const std::wstring& text, const std::wstring& caption) {
        MessageBoxW(nullptr, text.c_str(), caption.c_str(), S_OK);
    }
}