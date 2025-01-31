#pragma once

#include <string>

namespace LIEngine {

    namespace Utility {
        /// <summary>
        /// stringからwstringに変換
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        std::wstring ConvertString(const std::string& str);
        /// <summary>
        /// wstringからstringに変換
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        std::string ConvertString(const std::wstring& str);

    }

}