#include "Stage.h"

#include "MovingFloor.h"

void Stage::Initialize() {
    
    std::shared_ptr<Floor> floor0 = std::make_shared<Floor>();
    floors_.emplace_back(floor0);

    for (auto& floor : floors_) {
        floor->Initialize();
    }
   
    std::shared_ptr<Transform> transform = floor0->GetTransform();
    transform->translate = { 0.0f, -1.0f, 0.0f };
}

void Stage::Update() {

    for (auto& floor : floors_) {
        floor->Update();
    }
}
