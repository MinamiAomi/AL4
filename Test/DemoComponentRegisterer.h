#pragma once

#include "GameObject/ComponentRegisterer.h"

class DemoComponentRegisterer :
    public ComponentRegisterer {
public:
    DemoComponentRegisterer();
    std::shared_ptr<Component> Register(GameObject& gameObject, const std::string& id) const override;

};