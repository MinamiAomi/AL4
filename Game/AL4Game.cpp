#include "AL4Game.h"

#include "Scene/SceneManager.h"

#include "GameScene.h"

void AL4Game::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<GameScene>();


}

void AL4Game::OnFinalize() {
}
