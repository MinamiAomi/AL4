#pragma once
#include "GameObject/Component.h"

#include "Math/Camera.h"

class CameraComponent :
    public Component {
    COMPONENT_IMPL(CameraComponent);
public:
    CameraComponent();
    void Initialize() override;
    void Update() override;
    void Edit() override;
    void Export(nlohmann::json& json) const override;
    void Import(const nlohmann::json& json) override;

private:
    void MoveCamera();

    std::shared_ptr<Camera> camera_;
    Vector3 euler_;
};