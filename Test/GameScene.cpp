#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"

void GameScene::OnInitialize() {
    followCamera_ = std::make_shared<FollowCamera>();
    player_ = std::make_shared<Player>();
    ground = std::make_shared<Ground>();

    const char* kTestObjectNames[] = {
        "teapot",
        "bunny",
        "box",
        "sphere",
        "cylinder",
        "torus",
        "suzanne",
    };
    testObjects_.resize(_countof(kTestObjectNames));
    for (size_t i = 0; i < testObjects_.size(); ++i) {
        testObjects_[i] = std::make_shared<TestObject>();
        testObjects_[i]->Initialize(kTestObjectNames[i], Vector3(i * 3 - (float)testObjects_.size() * 0.5f * 3, 1.5f, 5.0f));
    }


    player_->SetFollowCamera(followCamera_);
    followCamera_->SetTarget(player_);

    followCamera_->Initialize();
    player_->Initialize();
    ground->Initialize();

}

void GameScene::OnUpdate() {
    player_->Update();
    ground->Update();
    followCamera_->Update();

    for (auto& testObject : testObjects_) {
        testObject->Update();
    }
}

void GameScene::OnFinalize() {
}
