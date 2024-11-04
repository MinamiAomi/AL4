#pragma once

#include <filesystem>

namespace SceneIO {
    void Load(const std::filesystem::path& path);
    void Save(const std::filesystem::path& path);
};