#pragma once
#include <string>
#include "GameObject.h"

class GameObjectFactory {
public:
    ~GameObjectFactory() {}
    virtual std::shared_ptr<GameObject> CreateGameObject(const std::string& id) = 0;
};
