#include "Stage.h"

struct FloorInitData {
    Vector3 base;
    float rotate;
    float movement;
    uint32_t moveCycle;
};

static const FloorInitData stageInits[] = {
    { {  0.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { {  0.0f, -1.0f, 10.0f }, 0.0f,         5.0f, 300  },
    { {  0.0f, -1.0f, 20.0f }, 0.0f,         0.0f,   0  },
    { { 10.0f, -1.0f, 20.0f }, Math::HalfPi, 5.0f, 300  },
    { { 20.0f, -1.0f, 20.0f }, 0.0f,         0.0f,  0  },
};

void Stage::Initialize() {
    
    enemy_ = std::make_shared<Enemy>();
    enemy_->Initialize({ 0.0f,0.0f,20.0f });

    floors_.resize(_countof(stageInits));

    uint32_t i = 0;
    for (auto& floor : floors_) {
        floor = std::make_shared<Floor>();
        floor->Initialize(stageInits[i].base, stageInits[i].rotate, stageInits[i].movement, stageInits[i].moveCycle);
        i++;
    }

    const uint32_t goalFloorIndex = 4;
    goal_ = std::make_shared<Goal>();
    goal_->transform.SetParent(&floors_[goalFloorIndex]->transform);
    goal_->transform.translate = { 0.0f,1.0f,0.0f };
    goal_->Initialize();
}

void Stage::Update() {
    enemy_->Update();
    for (auto& floor : floors_) {
        floor->Update();
    }
    goal_->Update();
}
