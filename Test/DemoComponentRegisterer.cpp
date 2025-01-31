#include "DemoComponentRegisterer.h"

#include "MeshComponent.h"

using namespace LIEngine;

DemoComponentRegisterer::DemoComponentRegisterer() :
    ComponentRegisterer({
        "MeshComponent"
        }) {

}

std::shared_ptr<Component> DemoComponentRegisterer::Register(GameObject& gameObject, const std::string& id) const {
    if (id == "MeshComponent") {
        return gameObject.AddComponent<MeshComponent>();
    }
    return std::shared_ptr<Component>();
}
