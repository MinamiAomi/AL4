#include "Ground.h"

#include "Graphics/ResourceManager.h"
#include "Graphics/RenderManager.h"

void Ground::Initialize() {
    model_ = std::make_unique<ModelInstance>();
    skydomeModel_ = std::make_unique<ModelInstance>();
    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    model_->SetModel(ResourceManager::GetInstance()->FindModel("Floor"));
    model_->SetReflection(true);

    skydomeModel_->SetModel(ResourceManager::GetInstance()->FindModel("skydome"));
    skydomeModel_->SetUseLighting(false);
    skydomeModel_->SetCastShadow(false);
    skydomeModel_->SetReciveShadow(false);
    skydomeModel_->SetWorldMatrix(Matrix4x4::MakeTranslation({ 0.0f, -10.0f, 0.0f }));
}

void Ground::Update() {

    transform.UpdateMatrix();
    model_->SetWorldMatrix(transform.worldMatrix);
}