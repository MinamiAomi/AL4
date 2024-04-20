#include "TitleObject.h"

#include "Framework/ResourceManager.h"

void TitleObject::Initialize() {
    model_ = std::make_unique<ModelInstance>();

    base_ = { 0.0f, 0.0f, -3.0f };
    transform.translate = base_;
    transform.rotate = Quaternion::identity;
    transform.scale = Vector3::one;

    model_->SetModel(ResourceManager::GetInstance()->FindModel("Title"));
    model_->SetIsActive(true);
    model_->SetCastShadow(false);
    //model_->SetReflection(true);
}

void TitleObject::Update(float time) {

    //transform.rotate = Quaternion::MakeForYAxis(Math::ToRadian) * transform.rotate;
    Vector3 offset = { 0.0f, 0.0f, std::sin(time * 3.1415926435f * 2.0f) * 2.0f  };
    transform.translate = base_ + offset;

    transform.UpdateMatrix();
    model_->SetWorldMatrix(transform.worldMatrix);
}

