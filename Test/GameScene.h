#pragma once
#include "Scene/BaseScene.h"

#include <memory>
#include <vector>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"

#include "FollowCamera.h"
#include "Player.h"
#include "Ground.h"
#include "TestObject.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<FollowCamera> followCamera_;
    std::shared_ptr<Player> player_;
    std::shared_ptr<Ground> ground;
    std::vector<std::shared_ptr<TestObject>> testObjects_;
};
