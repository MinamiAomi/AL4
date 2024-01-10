#include "TestObject.h"

#include "Graphics/ResourceManager.h"

void TestObject::Initialize(const std::string& name, const Vector3& position) {
    model_ = std::make_unique<ModelInstance>();

    transform.translate = position;
    transform.rotate = Quaternion::identity;
    transform.scale = Vector3::one;

    model_->SetModel(ResourceManager::GetInstance()->FindModel(name));
    model_->SetIsActive(true);
    //model_->SetReflection(true);
}

void TestObject::Update() {

    transform.rotate = Quaternion::MakeForYAxis(Math::ToRadian) * transform.rotate;

    transform.UpdateMatrix();
    model_->SetWorldMatrix(transform.worldMatrix);
}

