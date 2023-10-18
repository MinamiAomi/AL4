#include "Floor.h"

#include "Graphics/ResourceManager.h"

void Floor::Initialize() {
    transform_ = std::make_shared<Transform>();

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Floor"));
    model_->SetIsActive(true);
    model_->SetUseOutline(false);
}

void Floor::Update() {
    transform_->UpdateMatrix();
    model_->SetWorldMatrix(transform_->worldMatrix);
}
