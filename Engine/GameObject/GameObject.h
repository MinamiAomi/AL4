#pragma once
#include "Editer/EditerInterface.h"

#include <typeindex>
#include <typeinfo>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Component.h"
#include "Math/Transform.h"

class GameObject :
    public std::enable_shared_from_this<GameObject>, public SelectableInEditer {
    friend class GameObjectManager;
public:
    virtual ~GameObject() {}

    void RenderInInspectorView() override;

    void InitializeUninitializedComponents();
    void Update();

    void SetParent(const std::shared_ptr<GameObject>& gameObject);
    bool HasParent() const { return !parent_.expired(); }
    const std::weak_ptr<GameObject>& GetParent() const { return parent_; }
    const std::vector<std::weak_ptr<GameObject>>& GetChildren() const { return children_; }

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
        uninitializedComponents_.emplace_back(component);
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
        auto iter = componentList_.find(typeid(T));
        if (iter != componentList_.end()) {
            // 初期化されてないコンポーネントも削除
            uninitializedComponents_.erase(
                std::remove_if(uninitializedComponents_.begin(), uninitializedComponents_.end(), [&](const std::shared_ptr<Component>& component) {
                    return component == iter->second;
                    }),
                uninitializedComponents_.end());
            componentList_.erase(iter);
        }

    }

    void SetName(const std::string& name) { name_ = name; }
    const std::string& GetName() const { return name_; }

    void SetIsActive(bool isActive) { isActive_ = isActive; }
    bool IsActive() const { return isActive_; }

    Transform transform;

private:
    void AddChild(const std::shared_ptr<GameObject>& gameObject);
    void RemoveChild(const std::shared_ptr<GameObject>& gameObject);

    // 親
    std::weak_ptr<GameObject> parent_;
    // 子
    std::vector<std::weak_ptr<GameObject>> children_;
    // 未初期化のコンポーネント
    // 最初の更新で初期化される
    std::vector<std::shared_ptr<Component>> uninitializedComponents_;
    // コンポーネントリスト
    std::map<std::type_index, std::shared_ptr<Component>> componentList_;
    // 名前
    std::string name_;
    bool isActive_;
};