#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"

void GameScene::OnInitialize() {
    followCamera_ = std::make_shared<FollowCamera>();
    player_ = std::make_shared<Player>();
    floor_ = std::make_shared<Floor>();

    player_->SetFollowCamera(followCamera_);
    followCamera_->SetTarget(player_);

    followCamera_->Initialize();
    player_->Initialize();
    floor_->Initialize();

}

void GameScene::OnUpdate() {
    player_->Update();
    floor_->Update();
    followCamera_->Update();
}

void GameScene::OnFinalize() {
}
