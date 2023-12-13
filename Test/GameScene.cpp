#include "GameScene.h"

#include "Graphics/RenderManager.h"

void GameScene::OnInitialize() {
    camera_ = std::make_shared<Camera>();
    camera_->SetPosition(camera_->GetPosition() + Vector3{0.0f, 3.0f, -2.0f});
    camera_->UpdateMatrices();
    RenderManager::GetInstance()->SetCamera(camera_);

    floor_ = Model::Load("Resources/Floor/Floor.obj");
    teapot_ = Model::Load("Resources/Teapot/teapot.obj");
    bunny_ = Model::Load("Resources/Bunny/bunny.obj");

    floorModel_.SetModel(floor_);
    teapotModel_.SetModel(teapot_);
    teapotModel_.SetWorldMatrix(Matrix4x4::MakeTranslation({ 0.0f, 1.0f, 0.0f }));
    bunnyModel_.SetModel(bunny_);
    bunnyModel_.SetWorldMatrix(Matrix4x4::MakeTranslation({ 0.0f, 3.0f, 0.0f }));
}

void GameScene::OnUpdate() {
}

void GameScene::OnFinalize() {
}
