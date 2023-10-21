#include "GameScene.h"

#include "Collision/CollisionManager.h"

void GameScene::OnInitialize() {
    // 生成
    player_ = std::make_shared<Player>();
    followCamera_ = std::make_shared<FollowCamera>();
    //ground_ = std::make_shared<Ground>();
    skydome_ = std::make_shared<Skydome>();
    stage_ = std::make_shared<Stage>();
    
    // 初期化
    stage_->Initialize();
    player_->Initialize();
    followCamera_->Initialize();
    //ground_->Initialize();
    skydome_->Initialize();

    // セット
    player_->SetCamera(followCamera_);
    followCamera_->SetTarget(&player_->transform);
}

void GameScene::OnUpdate() {
    // 更新
    stage_->Update();
    player_->Update();
    //ground_->Update();
    skydome_->Update();

    // 当たり判定を取る
    CollisionManager::GetInstance()->CheckCollision();
    followCamera_->Update();
}

void GameScene::OnFinalize() {
   // CollisionManager::GetInstance()->ClearCollider();
}
