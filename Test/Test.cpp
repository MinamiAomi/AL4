#include "Test.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Model.h"

#include "TitleScene.h"
#include "GameScene.h"
#include "TestScene.h"
#include "SponzaScene.h"

void Test::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<SponzaScene>();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	resourceManager->AddModel("Title", Model::Load("Resources/title.obj"));
	resourceManager->AddModel("Player", Model::Load("Resources/Player/Player.obj"));
	resourceManager->AddModel("Floor", Model::Load("Resources/Floor/Floor.obj"));
	resourceManager->AddModel("teapot", Model::Load("Resources/Teapot/teapot.obj"));
	resourceManager->AddModel("bunny",  Model::Load("Resources/Bunny/bunny.obj"));
	resourceManager->AddModel("box",  Model::Load("Resources/box.obj"));
	resourceManager->AddModel("sphere",    Model::Load("Resources/sphere.obj"));
	resourceManager->AddModel("cylinder", Model::Load("Resources/cylinder.obj"));
	resourceManager->AddModel("torus",   Model::Load("Resources/torus.obj"));
	resourceManager->AddModel("suzanne", Model::Load("Resources/suzanne.obj"));
	resourceManager->AddModel("skydome", Model::Load("Resources/skydome.obj"));

	// カエルからだ
	resourceManager->AddModel("FrogBody", Model::Load("Resources/Model/frog/Body.obj"));
	// カエル頭
	resourceManager->AddModel("FrogHead", Model::Load("Resources/Model/frog/Head.obj"));
	// カエル左腕
	resourceManager->AddModel("FrogLeftArm", Model::Load("Resources/Model/frog/leftarm.obj"));
	// カエル右腕
	resourceManager->AddModel("FrogRightArm", Model::Load("Resources/Model/frog/rightarm.obj"));
	// カエル左脚
	resourceManager->AddModel("FrogLeftFoot", Model::Load("Resources/Model/frog/leftfoot.obj"));
	// カエル右脚
	resourceManager->AddModel("FrogRightFoot", Model::Load("Resources/Model/frog/rightfoot.obj"));
	// カエルカバン
	resourceManager->AddModel("FrogBag", Model::Load("Resources/Model/frog/bag.obj"));

	resourceManager->AddModel("Sponza", Model::Load("Resources/Sponza/sponza.obj"));

	//auto model = Model::Load("Resources/CornellBox/CornellBox-Original.obj");
}

void Test::OnFinalize() {
}
