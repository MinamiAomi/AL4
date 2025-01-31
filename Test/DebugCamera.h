#pragma once

#include <memory>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"

class DebugCamera {
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    // ゲッター

    const std::shared_ptr<LIEngine::Camera>& GetCamera() const { return camera_; }

private:
    LIEngine::Transform transform;
    std::shared_ptr<LIEngine::Camera> camera_;
    LIEngine::Vector3 eulerAngle_;
};