#pragma once

#include <typeindex>
#include <typeinfo>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Component.h"
#include "Math/Transform.h"

class GameObject {
    friend class GameObjectManager;
public:
    virtual ~GameObject() {}

    void InitializeUninitializedComponents();
    void Update();
    void ImguiDraw();

    // コンポーネントを追加
    template<class T>
    std::shared_ptr<T> AddComponent() {
        static_assert(std::is_base_of<Component, T>::value, "Componentが継承されていません。");
        auto iter = componentList_.find(typeid(T));
        if (iter != componentList_.end()) {
            return std::static_pointer_cast<T>(iter->second);
        }
        std::shared_ptr<T> component = std::make_shared<T>();
        component->gameObject_ = this;
        uninitializedComponents_.emplace_back(component.get());
        componentList_.emplace(typeid(T), component);
        return component;
    }

    // コンポーネントを取得
    template<class T>
    std::shared_ptr<T> GetComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "Componentが継承されていません。");
        auto iter = componentList_.find(typeid(T));
        if (iter != componentList_.end()) {
            return std::static_pointer_cast<T>(iter->second);
        }
        return std::shared_ptr<T>();
    }

    // コンポーネントを削除
    template<class T>
    void RemoveComponent() {
        static_assert(std::is_base_of<Component, T>::value, "Componentが継承されていません。");
        componentList_.erase(typeid(T));
    }
    
    void SetName(const std::string& name) { name_ = name; }
    const std::string& GetName() const { return name_; }

    void SetIsActive(bool isActive) { isActive_ = isActive; }
    bool IsActive() const { return isActive_; }

    Transform transform;

private:
    std::vector<Component*> uninitializedComponents_;
    std::map<std::type_index, std::shared_ptr<Component>> componentList_;
    std::string name_;
    bool isActive_;
};