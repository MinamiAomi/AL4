#include "GameObjectManager.h"

void GameObjectManager::Update() {
    for (auto& gameObject : gameObjects_) {
        gameObject->Update();
    }
}
