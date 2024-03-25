#include "TestScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"

void TestScene::OnInitialize() {

    camera_ = std::make_shared<Camera>();
    camera_->SetPosition({ 0.0f, 0.0f, -35.0f });
    camera_->UpdateMatrices();
    RenderManager::GetInstance()->SetCamera(camera_);

    sunLight_ = std::make_shared<DirectionalLight>();
    sunLight_->direction = Vector3(1.0f, -1.0f, 1.0f).Normalized();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    testObject_.Initialize("pbr", {});
    testObject_.transform.rotate = Quaternion::MakeForXAxis(-90.0f * Math::ToRadian);
    testObject_.transform.translate = { 40.0f, 0.0f, 0.0f };
    testObject_.Update();
    boxObject_.Initialize("box", {});
    boxObject_.Update();

    euler_.x = Math::ToRadian;
}

void TestScene::OnUpdate() {

  

    Input* input = Input::GetInstance();

    auto mouseMoveX = input->GetMouseMoveX();
    auto mouseMoveY = input->GetMouseMoveY();
    auto wheel = input->GetMouseWheel();

    Quaternion rotate = camera_->GetRotate();
    Vector3 position = camera_->GetPosition();

    Vector3 diffPosition;

    if (input->IsMousePressed(1)) {
        constexpr float rotSpeed = Math::ToRadian * 0.1f;
        euler_.x += rotSpeed * static_cast<float>(mouseMoveY);
        euler_.y += rotSpeed * static_cast<float>(mouseMoveX);
    }
    else if (input->IsMousePressed(2)) {
        Vector3 cameraX = rotate.GetRight() * (-static_cast<float>(mouseMoveX) * 0.01f);
        Vector3 cameraY = rotate.GetUp() * (static_cast<float>(mouseMoveY) * 0.01f);
        diffPosition += cameraX + cameraY;
    }
    else if (wheel != 0) {
        Vector3 cameraZ = rotate.GetForward() * (static_cast<float>(wheel / 120) * 0.5f);
        diffPosition += cameraZ;
    }


    camera_->SetPosition(position + diffPosition);
    camera_->SetRotate(Quaternion::MakeFromEulerAngle(euler_));
    camera_->UpdateMatrices();

    sunLight_->DrawImGui("SunLight");
    //testObject_.DrawImGui("Sphere");
}

void TestScene::OnFinalize() {
}
