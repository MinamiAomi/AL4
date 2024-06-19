#pragma once

#include <string>
#include <memory>
#include <list>

#include "GameObject.h"
#include "GameObjectFactory.h"

class GameObjectManager {
public:
    void Update();

    void AddGameObject(const std::shared_ptr<GameObject>& gameObject) { gameObjects_.emplace_back(gameObject); }
    const std::list<std::shared_ptr<GameObject>>& GetGameObjects() const { return gameObjects_; }

    template<class T>
    void SetFactory() {
        static_assert(std::is_base_of<GameObjectFactory, T>::value, "継承されていません。");
        factory_ = std::make_unique<T>(*this);
    }

    GameObjectFactory* GetFactory() const { return factory_.get(); }

private:
    std::list<std::shared_ptr<GameObject>> gameObjects_;
    std::unique_ptr<GameObjectFactory> factory_;

};