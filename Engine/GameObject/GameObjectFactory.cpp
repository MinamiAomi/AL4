#include "GameObjectFactory.h"

namespace LIEngine {

    std::shared_ptr<GameObject> DefaultGameObjectFactory::CreateGameObject(const std::string& id) const {
        id;
        return std::make_shared<GameObject>();
    }

    void DefaultGameObjectFactory::CreateGameObjectFromEditer() {}

}