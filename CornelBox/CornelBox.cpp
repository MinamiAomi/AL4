#include "CornelBox.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Model.h"

#include "GameScene.h"

void CornelBox::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<GameScene>();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	resourceManager;
	auto model = Model::Load("Resources/CornellBox/CornellBox-Original.obj");
}

void CornelBox::OnFinalize() {
}
