#include "GameScene.h"

#include "Graphics/RenderManager.h"

void GameScene::OnInitialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);

    floor_ = Model::Load("Resources/Floor/Floor.obj");
    teapot_ = Model::Load("Resources/Teapot/teapot.obj");
    bunny_ = Model::Load("Resources/Bunny/bunny.obj");


    floorModel_.SetModel(floor_);
    teapotModel_.SetModel(teapot_);
    bunnyModel_.SetModel(bunny_);
}

void GameScene::OnUpdate() {
}

void GameScene::OnFinalize() {
}
