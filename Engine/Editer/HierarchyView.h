#pragma once
#include "EditerView.h"

#include <memory>
#include <string>

class GameObject;
class GameObjectManager;

namespace Editer {

    class HierarchyView :
        public View {
    public:
        using View::View;

        void Render(const GameObjectManager& gameObjectManager);

    private:
        void RenderGameObject(const std::shared_ptr<GameObject>& gameObject, int32_t depth);

        std::string searchString_;
    };

}