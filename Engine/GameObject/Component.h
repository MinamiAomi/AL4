#pragma once

#include <string>

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

    GameObject& GetGameObject() { return *gameObject_; }
    const GameObject& GetGameObject() const { return *gameObject_; }

    bool IsActive() const { return isActive_; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }

private:
    GameObject* gameObject_ = nullptr;
    bool isActive_ = true;
};