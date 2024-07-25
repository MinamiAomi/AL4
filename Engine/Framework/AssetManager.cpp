#include "AssetManager.h"

AssetManager* AssetManager::GetInstance() {
    static AssetManager instance;
    return &instance;
}

void AssetManager::Add(const std::shared_ptr<Asset>& resource) {
    std::lock_guard<std::mutex> lock(mutex_);
    assetList_.emplace_back(resource);
}

std::shared_ptr<Asset> AssetManager::Find(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = std::find_if(assetList_.begin(), assetList_.end(), [name](std::shared_ptr<Asset>& resource) {
        return resource->GetName() == name;
        });
    if (iter != assetList_.end()) {
        return *iter;
    }
    return std::shared_ptr<Asset>();
}


void AssetManager::Remove(const std::shared_ptr<Asset>& resource) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = std::remove(assetList_.begin(), assetList_.end(), resource);
    assetList_.erase(iter);
}
