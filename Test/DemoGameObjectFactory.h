#pragma once
#include "GameObject/GameObjectFactory.h"

class DemoGameObjectFactory :
    public GameObjectFactory {
public:
    using GameObjectFactory::GameObjectFactory;

    std::shared_ptr<GameObject> CreateGameObject(const std::string& id) const override;
    void CreateGameObjectFromEditer() override;

};

