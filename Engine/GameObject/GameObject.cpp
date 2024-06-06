#include "GameObject.h"

void GameObject::Update() {
    // 未初期化のコンポーネントを初期化
    for (auto component : uninitializedComponents_) {
        component->Initialize();
    }
    uninitializedComponents_.clear();

    // すべてのコンポーネントを更新
    for (auto& component : componentList_) {
        component.second->Update();
    }
    transform.UpdateMatrix();
}
