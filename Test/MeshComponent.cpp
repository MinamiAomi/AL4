#include "MeshComponent.h"

#include "GameObject/GameObject.h"

#include "Framework/ResourceManager.h"

void MeshComponent::Initialize() {
    auto resourceManager = ResourceManager::GetInstance();
    model_.SetModel(resourceManager->FindModel(modelName_));
    auto& gameObject = GetGameObject();
    gameObject.transform.UpdateMatrix();
    model_.SetWorldMatrix(gameObject.transform.worldMatrix);
}
