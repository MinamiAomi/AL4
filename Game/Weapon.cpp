#include "Weapon.h"

#include "Graphics/ResourceManager.h"
#include "CollisionAttribute.h"
#include "Collision/CollisionManager.h"


void Weapon::Initialize() {
    SetName("Player");

    transform.translate = {0.0f,2.0f,0.0f};
    transform.scale = Vector3::one;
    transform.UpdateMatrix();

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Weapon"));
    model_->SetIsActive(true);
    model_->SetOutlineWidth(0.02f);
    model_->SetOutlineColor({ 0.0f,0.0f,0.0f });

    collider_ = std::make_unique<SphereCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Weapon");
    collider_->SetCenter(transform.translate + colliderOffset_);
    collider_->SetRadius(0.15f);
    collider_->SetCollisionAttribute(CollisionAttribute::Player);
    collider_->SetCollisionMask(~CollisionAttribute::Player);
}

void Weapon::Update() {
    UpdateTransform();
}

void Weapon::UpdateTransform() {
    transform.UpdateMatrix();
    Vector3 worldPosition = transform.worldMatrix.GetTranslate();
    Vector3 worldColliderOffset = colliderOffset_ * transform.worldMatrix;
    collider_->SetCenter(worldPosition + worldColliderOffset);
    model_->SetWorldMatrix(transform.worldMatrix);
}
