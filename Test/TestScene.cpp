#include "TestScene.h"

#include "Framework/Engine.h"
#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Framework/AssetManager.h"
#include "Graphics/Core/TextureLoader.h"
#include "LevelLoader.h"

void TestScene::OnInitialize() {

    camera_ = std::make_shared<Camera>();
    camera_->SetPosition({ 0.0f, 1.0f, -5.0f });
    camera_->SetRotate(Quaternion::MakeLookRotation({ 0.0f, -1.0f, 5.0f }));
    camera_->UpdateMatrices();
    RenderManager::GetInstance()->SetCamera(camera_);

    sunLight_ = std::make_shared<DirectionalLight>();
    sunLight_->direction = kInitialSunLightDirection;
    RenderManager::GetInstance()->SetSunLight(sunLight_);


    Vector3 offset = { 50.0f, 20.0f, 0.0f };
    auto sphereModel = AssetManager::GetInstance()->modelMap.Get("sphere");
    for (uint32_t row = 0; row < kRowCount; ++row) {
        for (uint32_t column = 0; column < kColumnCount; ++column) {
            auto& sphere = spheres_[row][column];
            sphere.material = std::make_shared<Material>();
            sphere.material->albedo = { 1.0f, 1.0f, 1.0f };
            sphere.material->metallic = (float)row / (kRowCount - 1);
            sphere.material->roughness = (float)column / (kColumnCount - 1);
            sphere.model.SetModel(sphereModel->Get());
            sphere.model.SetMaterial(sphere.material);
            sphere.model.SetWorldMatrix(Matrix4x4::MakeTranslation(Vector3{ -15.0f + column * 3.0f, -15.0f + row * 3.0f, 0.0f } + offset));
            sphere.model.SetBeReflected(false);
        }
    }

    //player_.Initialize();
    LevelLoader::Load("Resources/scene.json", *Engine::GetGameObjectManager());
}

void TestScene::OnUpdate() {

    Engine::GetGameObjectManager()->Update();

    //sunLight_->DrawImGui("SunLight");
    //testObject_.DrawImGui("Sphere");
}

void TestScene::OnFinalize() {

}
