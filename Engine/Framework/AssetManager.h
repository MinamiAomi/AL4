#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Asset.h"
#include "TextureAsset.h"
#include "ModelAsset.h"
#include "MaterialAsset.h"
#include "AnimationAsset.h"
#include "SoundAsset.h"

class Model;
class TextureResource;
class Sound;
class Animation;

class AssetManager {
public:
    using AssetList = std::list<std::shared_ptr<Asset>>;
    template<typename T>
    using AssetMap = std::map<std::string, std::shared_ptr<T>>;

    static AssetManager* GetInstance();

    void AddModel(const std::string& name, const std::shared_ptr<Model>& model) { modelMap_.emplace(std::make_pair(name, model)); }
    std::shared_ptr<Model> FindModel(const std::string& name) const { return modelMap_.at(name); }
    
    void AddTexture(const std::string& name, const std::shared_ptr<TextureResource>& texture) {
        textureMap_.emplace(std::make_pair(name, texture));
    }
    std::shared_ptr<TextureResource> FindTexture(const std::string& name) const { return textureMap_.at(name); }

    void AddSound(const std::string& name, const std::shared_ptr<Sound>& sound) {
        soundMap_.emplace(std::make_pair(name, sound));
    }
    std::shared_ptr<Sound> FindSound(const std::string& name) const { return soundMap_.at(name); }

    void AddAnimation(const std::string& name, const std::shared_ptr<Animation>& animation) {
        animationMap_.emplace(std::make_pair(name, animation));
    }
    std::shared_ptr<Animation> FindAnimation(const std::string& name) const { return animationMap_.at(name); }



    void Add(const std::shared_ptr<Asset>& resource);
    std::shared_ptr<Asset> Find(const std::string& name);

    void Remove(const std::shared_ptr<Asset>& resource);
    const AssetList& GetAssetList() const { return assetList_; }

private:
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::map<std::string, std::shared_ptr<Model>> modelMap_;
    std::map<std::string, std::shared_ptr<TextureResource>> textureMap_;
    std::map<std::string, std::shared_ptr<Sound>> soundMap_;
    std::map<std::string, std::shared_ptr<Animation>> animationMap_;

    AssetMap<TextureAsset> textures_;
    AssetMap<ModelAsset> models_;
    AssetMap<MaterialAsset> materials_;
    AssetMap<AnimationAsset> animations_;
    AssetMap<SoundAsset> sounds_;
    AssetList assetList_;

    std::mutex mutex_;
};