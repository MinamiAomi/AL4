#pragma once

#include <memory>
#include <string>

#include "Externals/nlohmann/json.hpp"

#ifndef STRINGIFY
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#endif // STRINGIFY

#ifndef COMPONENT_IMPL
#define COMPONENT_IMPL(ClassName) \
    public:\
        std::string GetComponentName() const override { return STRINGIFY(ClassName); }\
    private:
#endif // !COMPONENT_IMPL

class GameObject;

class Component {
    friend class GameObject;
public:
    virtual ~Component() = 0 {}
    
    virtual std::string GetComponentName() const = 0 {}
    virtual void Initialize() {}
    virtual void Update() {}
    virtual void Edit() {}
    virtual void Export(nlohmann::json&) const {}
    virtual void Import(const nlohmann::json&) {}

    std::shared_ptr<GameObject> GetGameObject() { return gameObject_.lock(); }
    std::shared_ptr<GameObject> GetGameObject() const { return gameObject_.lock(); }

    bool IsActive() const { return isActive_; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }

private:
    std::weak_ptr<GameObject> gameObject_;
    bool isActive_ = true;
};