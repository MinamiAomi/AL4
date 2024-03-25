#include "TitleScene.h"

#include "Scene/SceneManager.h"
#include "Graphics/RenderManager.h"
#include "Graphics/ResourceManager.h"
#include "Input/Input.h"

#include "GameScene.h"

void TitleScene::OnInitialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);
    camera_->SetPosition({ 0.0f, -3.0f, -6.0f });
    camera_->SetRotate(Quaternion::MakeLookRotation(-camera_->GetPosition()));

    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);
    sunLight_->direction = Vector3(0.0f, -1.0f, 1.0f).Normalized();

    titleObject_ = std::make_shared<TitleObject>();
    titleObject_->Initialize();

    panel_ = std::make_unique<ModelInstance>();
    panel_->SetModel(ResourceManager::GetInstance()->FindModel("box"));
    panel_->SetWorldMatrix(Matrix4x4::MakeAffineTransform({ 50.0f, 50.0f, 1.0f }, Quaternion::identity, { 0.0f, 0.0f, 0.5f }));

    cycle_ = 300  ;
    time_ = 0.0f;

}

void TitleScene::OnUpdate() {

    time_ += 1.0f / cycle_;
    if (time_ > 1.0f) { time_ -= 1.0f; }

    
    titleObject_->Update(time_);
    camera_->UpdateMatrices();


    bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
    if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
        SceneManager::GetInstance()->ChangeScene<GameScene>();
    }
}

void TitleScene::OnFinalize() {
}
