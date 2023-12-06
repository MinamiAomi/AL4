#pragma once
#include "Scene/BaseScene.h"

#include <memory>

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
};
