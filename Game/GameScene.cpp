#include "GameScene.h"

void GameScene::OnInitialize() {
    // 生成
    player_ = std::make_shared<Player>();
    followCamera_ = std::make_shared<FollowCamera>();
    //ground_ = std::make_shared<Ground>();
    skydome_ = std::make_shared<Skydome>();
    stage_ = std::make_shared<Stage>();
    
    // 初期化
    player_->Initialize();
    followCamera_->Initialize();
    //ground_->Initialize();
    skydome_->Initialize();
    stage_->Initialize();

    // セット
    player_->SetCamera(followCamera_->GetCamera());
    followCamera_->SetTarget(player_->GetTransform());
}

void GameScene::OnUpdate() {
    // 更新
    player_->Update();
    followCamera_->Update();
    //ground_->Update();
    skydome_->Update();
    stage_->Update();
}

void GameScene::OnFinalize() {
}
