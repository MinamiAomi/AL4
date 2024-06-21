#include "Debug.h"

#include <Windows.h>

#include "Framework/Engine.h"
#ifdef ENABLE_IMGUI
#include "Editer/EditerManager.h"
#endif // ENABLE_IMGUI


namespace Debug {
    void Log(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        int len = std::vsnprintf(nullptr, 0, fmt, args) + 1;
        std::vector<char> buffer(len);
        std::vsnprintf(buffer.data(), len, fmt, args);
        Log(std::string(buffer.data(), buffer.size() - 1));
        va_end(args);
    }
    void Log(const std::string& str) {
        OutputDebugStringA(str.c_str());
#ifdef ENABLE_IMGUI
        Engine::GetEditerManager()->GetConsoleView().AddLog(Editer::LogType::Normal, str);
#endif // ENABLE_IMGUI

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