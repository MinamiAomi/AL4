#pragma once

#include <filesystem>

#include "GameObject/GameObjectManager.h"


namespace LevelLoader {
    void Load(const std::filesystem::path& path, GameObjectManager& gameObjectManager);
}
