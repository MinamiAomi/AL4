#include "Test.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Framework/ResourceManager.h"
#include "Graphics/Model.h"
#include "Audio/Sound.h"
#include "Graphics/Animation.h"

#include "TestScene.h"

void Test::OnInitialize() {
    SceneManager* sceneManager = SceneManager::GetInstance();
    //シーン設定
    sceneManager->ChangeScene<TestScene>(false);

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    resourceManager->AddModel("pbr", Model::Load("Resources/pbr/pbr.gltf"));
    //resourceManager->AddModel("room", Model::Load("Resources/PBRRoom/pbrRoom.gltf"));
    resourceManager->AddModel("door_frame", Model::Load("Resources/Door/Frame.gltf"));
    resourceManager->AddModel("door_panel", Model::Load("Resources/Door/Panel.gltf"));
    resourceManager->AddModel("door_knob", Model::Load("Resources/Door/Knob.gltf"));
    resourceManager->AddAnimation("door_frame", Animation::Load("Resources/Door/Frame.gltf"));
    resourceManager->AddAnimation("door_panel", Animation::Load("Resources/Door/Panel.gltf"));
    resourceManager->AddAnimation("door_knob", Animation::Load("Resources/Door/Knob.gltf"));

}

void Test::OnFinalize() {
}
