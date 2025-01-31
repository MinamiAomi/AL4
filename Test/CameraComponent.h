#pragma once
#include "GameObject/Component.h"

#include "Math/Camera.h"

class CameraComponent :
    public LIEngine::Component {
    COMPONENT_IMPL(CameraComponent);
public:
    CameraComponent();
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;
    /// <summary>
    /// エディターで使用される
    /// </summary>
    void Edit() override;
    /// <summary>
    /// jsonにエクスポート
    /// </summary>
    /// <param name="json"></param>
    void Export(nlohmann::json& json) const override;
    /// <summary>
    /// jsonにインポート
    /// </summary>
    /// <param name="json"></param>
    void Import(const nlohmann::json& json) override;

private:
    void MoveCamera();

    std::shared_ptr<LIEngine::Camera> camera_;
    LIEngine::Vector3 euler_;
};