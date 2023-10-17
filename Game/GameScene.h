#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Player.h"
#include "FollowCamera.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<Player> player_;
    std::shared_ptr<FollowCamera> followCamera_;

};
