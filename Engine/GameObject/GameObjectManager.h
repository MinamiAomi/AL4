#pragma once

#include <string>
#include <memory>
#include <list>

#include "GameObject.h"

class GameObjectManager {
public:
    void Update();

    void AddGameObject(const std::shared_ptr<GameObject>& gameObject) { gameObjects_.emplace_back(gameObject); }
    const std::list<std::shared_ptr<GameObject>>& GetGameObjects() const { return gameObjects_; }

private:
    std::list<std::shared_ptr<GameObject>> gameObjects_;
};