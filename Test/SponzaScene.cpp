#include "SponzaScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Scene/SceneManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/ParticleManager.h"

void SponzaScene::OnInitialize() {
    debugCamera_ = std::make_shared<DebugCamera>();
    debugCamera_->Initialize();

    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);
    sunLight_->direction = Vector3(0.1f, -1.0f, 0.1f).Normalized();

    testObjects_.emplace_back(std::make_shared<TestObject>());
    testObjects_.back()->Initialize("Sponza", {});
    testObjects_.back()->transform.scale = { 0.05f, 0.05f, 0.05f };
    testObjects_.back()->SetRotate(false);

    testObjects_.emplace_back(std::make_shared<TestObject>());
    testObjects_.back()->Initialize("sphere", { 0.0f, 1.0f, 0.0f});
    testObjects_.back()->SetRotate(false);

    const char* kTestObjectNames[] = {
            "teapot",
            "bunny",
            "box",
            "cylinder",
            "torus",
            "suzanne",
    };
    testObjects_.resize(_countof(kTestObjectNames));
    for (size_t i = 1; i < testObjects_.size(); ++i) {
        testObjects_[i] = std::make_shared<TestObject>();
        testObjects_[i]->Initialize(kTestObjectNames[i], Vector3(i * 3 - (float)testObjects_.size() * 0.5f * 3, 1.0f, 5.0f));
    }

    particleTimer_ = 0;
}

void SponzaScene::OnUpdate() {

    for (auto& testObject : testObjects_) {
        testObject->Update();
    }

    debugCamera_->Update();

    particleTimer_ += 1;
    if (particleTimer_ > particleConstant_.spawnCycle) {
        particleTimer_ = 0;
        auto particleManager = ParticleManager::GetInstance();
        std::list<Particle> particles;
        for (int32_t i = 0; i < particleConstant_.spawnCount; ++i) {
            Particle particle;
            particle.position = { 0.0f, 1.0f, -2.0f };       // 位置
            particle.velocity = Vector3(rng_.NextFloatUnit() * 2.0f - 1.0f, 2.0f, rng_.NextFloatUnit() * 2.0f - 1.0f).Normalized() * particleConstant_.speed;       // 速度
            particle.startSize = 0.1f;        // 開始時の大きさ
            particle.endSize = 0.0f;          // 終了時の大きさ
            particle.startColor = Vector3(1.0f, 1.0f, 1.0f);      // 開始時の色
            particle.endColor = Vector3(1.0f, 1.0f, 1.0f);       // 終了時の色
            particle.startAlpha = 1.0f;       // 開始時の透明度
            particle.endAlpha = 0.0f;         // 終了時の透明度
            particle.existenceTime = 0; // 存在時間
            particle.lifeTime = 300;      // 寿命
            particles.emplace_back(particle);
        }
        particleManager->AddParticles(std::move(particles));
    }

#ifdef ENABLE_IMGUI
    if (ImGui::TreeNode("SunLight")) {
        ImGui::DragFloat3("Direction", &sunLight_->direction.x, 0.01f, -1.0f, 1.0f);
        sunLight_->direction = sunLight_->direction.Normalized();
        ImGui::ColorEdit3("Color", &sunLight_->color.x);
        ImGui::DragFloat("Intensity", &sunLight_->intensity, 0.01f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Particle")) {
        
        ImGui::SliderInt("SpawnCycle", &particleConstant_.spawnCycle, 1, 10);
        ImGui::SliderInt("SpawnCount", &particleConstant_.spawnCount, 1, 10);
        ImGui::TreePop();
    }
#endif // ENABLE_IMGUI

    ParticleManager::GetInstance()->Update();
}

void SponzaScene::OnFinalize() {

}
