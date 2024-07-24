#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Resource.h"

class Model;
class Texture;
class Sound;
class Animation;

class ResourceManager {
public:
    static ResourceManager* GetInstance();

    void AddModel(const std::string& name, const std::shared_ptr<Model>& model) { modelMap_.emplace(std::make_pair(name, model)); }
    std::shared_ptr<Model> FindModel(const std::string& name) const { return modelMap_.at(name); }
    
    void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
        textureMap_.emplace(std::make_pair(name, texture));
    }
    std::shared_ptr<Texture> FindTexture(const std::string& name) const { return textureMap_.at(name); }

    void AddSound(const std::string& name, const std::shared_ptr<Sound>& sound) {
        soundMap_.emplace(std::make_pair(name, sound));
    }
    std::shared_ptr<Sound> FindSound(const std::string& name) const { return soundMap_.at(name); }

    void AddAnimation(const std::string& name, const std::shared_ptr<Animation>& animation) {
        animationMap_.emplace(std::make_pair(name, animation));
    }
    std::shared_ptr<Animation> FindAnimation(const std::string& name) const { return animationMap_.at(name); }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::shared_ptr<Model>> modelMap_;
    std::map<std::string, std::shared_ptr<Texture>> textureMap_;
    std::map<std::string, std::shared_ptr<Sound>> soundMap_;
    std::map<std::string, std::shared_ptr<Animation>> animationMap_;
};