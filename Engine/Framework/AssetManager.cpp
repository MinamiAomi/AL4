#include "AssetManager.h"

namespace LIEngine {

    AssetManager* AssetManager::GetInstance() {
        static AssetManager instance;
        return &instance;
    }

    void AssetManager::Clear() {
        textureMap.Clear();
        modelMap.Clear();
        materialMap.Clear();
        animationMap.Clear();
        soundMap.Clear();
    }

}