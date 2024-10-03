#include "AssetManager.h"

AssetManager* AssetManager::GetInstance() {
    static AssetManager instance;
    return &instance;
}

