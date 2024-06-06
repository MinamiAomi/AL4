#include "Test.h"

#include <memory>
#include <fstream>

#include "Externals/nlohmann/json.hpp"
#include "Scene/SceneManager.h"
#include "Framework/ResourceManager.h"
#include "Graphics/Model.h"
#include "Audio/Sound.h"
#include "Graphics/Animation.h"
#include "Graphics/Sprite.h"
#include "Debug/Debug.h"

#include "TestScene.h"

namespace {
    const char kResourceAssociationFile[] = "Resources/Association.json";
}

void Test::OnInitialize() {
    SceneManager* sceneManager = SceneManager::GetInstance();
    //シーン設定
    sceneManager->ChangeScene<TestScene>(false);

    LoadResource();
}

void Test::OnFinalize() {
}

void Test::LoadResource() {
    std::ifstream file;
    file.open(kResourceAssociationFile);
    assert(file.is_open());

    nlohmann::json json;
    file >> json;
    file.close();


    ResourceManager* resourceManager = ResourceManager::GetInstance();
    for (auto& texture : json["Texture"].items()) {
#ifdef _DEBUG
        auto duration = Debug::ElapsedTime([&]() {
#endif
            resourceManager->AddTexture(texture.key(), Texture::Load("Resources/" + texture.value().get<std::string>()));
#ifdef _DEBUG
            });
        std::stringstream str;
        str << "LoadTexture : " << texture.key() << " - " << duration << "\n";
        OutputDebugStringA(str.str().c_str());
#endif
    }
    for (auto& model : json["Model"].items()) {
#ifdef _DEBUG
        auto duration = Debug::ElapsedTime([&]() {
#endif
            resourceManager->AddModel(model.key(), Model::Load("Resources/" + model.value().get<std::string>()));
#ifdef _DEBUG
            });
        std::stringstream str;
        str << "LoadModel : " << model.key() << " - " << duration << "\n";
        OutputDebugStringA(str.str().c_str());
#endif
    }


    for (auto& sound : json["Sound"].items()) {
#ifdef _DEBUG
        auto duration = Debug::ElapsedTime([&]() {
#endif
            resourceManager->AddSound(sound.key(), Sound::Load("Resources/" + sound.value().get<std::string>()));
#ifdef _DEBUG
            });
        std::stringstream str;
        str << "LoadSound : " << sound.key() << " - " << duration << "\n";
        OutputDebugStringA(str.str().c_str());
#endif
    }
    for (auto& animation : json["Animation"].items()) {
#ifdef _DEBUG
        auto duration = Debug::ElapsedTime([&]() {
#endif
            resourceManager->AddAnimation(animation.key(), Animation::Load("Resources/" + animation.value().get<std::string>()));
#ifdef _DEBUG
            });
        std::stringstream str;
        str << "LoadAnimation : " << animation.key() << " - " << duration << "\n";
        OutputDebugStringA(str.str().c_str());
#endif
    }
}
