#include "AL4Game.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ToonModel.h"

#include "GameScene.h"

void AL4Game::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<GameScene>();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	
	// プレイヤー
	std::shared_ptr<ToonModel> toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/player.obj"));
	resourceManager->AddToonModel("Player", toonModel);
	// スカイドーム
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/skydome.obj"));
	resourceManager->AddToonModel("Skydome", toonModel);
	// 地面
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/ground.obj"));
	resourceManager->AddToonModel("Ground", toonModel);
	// ステージ床
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/floor.obj"));
	resourceManager->AddToonModel("Floor", toonModel);
}

void AL4Game::OnFinalize() {
}
