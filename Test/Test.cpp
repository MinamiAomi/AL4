#include "Test.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Framework/ResourceManager.h"
#include "Graphics/Model.h"
#include "Audio/Sound.h"

#include "TitleScene.h"
#include "GameScene.h"
#include "TestScene.h"
#include "SponzaScene.h"

void Test::OnInitialize() {
    SceneManager* sceneManager = SceneManager::GetInstance();
    //シーン設定
    sceneManager->ChangeScene<TestScene>(false);

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    resourceManager->AddModel("pbr", Model::Load("Resources/pbr/pbr.gltf"));
    resourceManager->AddModel("box", Model::Load("Resources/test/test.gltf"));

    resourceManager->AddSound("sazanami", Sound::Load("Resources/sazanami.mp3"));
}

void Test::OnFinalize() {
}
