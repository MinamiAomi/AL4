#include "GameScene.h"

void GameScene::OnInitialize() {
    player_ = std::make_shared<Player>();
    followCamera_ = std::make_shared<FollowCamera>();


    player_->Initialize();
    followCamera_->Initialize();

    player_->SetCamera(followCamera_->GetCamera());
    followCamera_->SetTarget(player_->GetTransform());
}

void GameScene::OnUpdate() {

    player_->Update();
    followCamera_->Update();

}

void GameScene::OnFinalize() {
}
