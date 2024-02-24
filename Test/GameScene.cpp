#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Scene/SceneManager.h"

void GameScene::OnInitialize() {
    sunLight_ = std::make_shared<DirectionalLight>();
    debugCamera_ = std::make_shared<DebugCamera>();
    map_ = std::make_shared<Map>();

    sunLight_->direction = Vector3(0.5f, -1.0f, 0.5f).Normalized();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    debugCamera_->Initialize();

    uint32_t xMax = 30;
    uint32_t yMax = 10;
    uint32_t zMax = 30;

    map_->Initialize(xMax, yMax, zMax);

    for (uint32_t y = 0; y < yMax; ++y) {
        for (uint32_t z = 0; z < zMax; ++z) {
            for (uint32_t x = 0; x < xMax; ++x) {
                if ((x + z) + 1 > y) {
                    map_->GetTile(x, y, z) = Map::kNormalTile;
                }
            }
        }
    }
    map_->SetupModel();
}

void GameScene::OnUpdate() {


    debugCamera_->Update();
}

void GameScene::OnFinalize() {
}
