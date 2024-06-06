#pragma once

#include <string>

class GameObject;

class Component {
    friend class GameObject;
public:
    virtual ~Component() = 0 {}

    virtual void Initialize() {}
    virtual void Update() {}


    GameObject& GetGameObject() { return *gameObject_; }
    const GameObject& GetGameObject() const { return *gameObject_; }

    bool IsActive() const { return isActive_; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }

private:
    GameObject* gameObject_ = nullptr;
    bool isActive_ = true;
};