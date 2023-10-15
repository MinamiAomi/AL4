#include "OneButton.h"

#include "Scene/SceneManager.h"
#include "TitleScene.h"
#include"ImGame.h"

void OneButton::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<InGame>();


}

void OneButton::OnFinalize() {
}
