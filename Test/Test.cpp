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
	sceneManager->ChangeScene<TestScene>(false);

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	//resourceManager->AddModel("Title", Model::Load("Resources/title.obj"));
	//resourceManager->AddModel("Player", Model::Load("Resources/Player/Player.obj"));
	//resourceManager->AddModel("Floor", Model::Load("Resources/Floor/Floor.obj"));
	//resourceManager->AddModel("teapot", Model::Load("Resources/Teapot/teapot.obj"));
	//resourceManager->AddModel("bunny",  Model::Load("Resources/Bunny/bunny.obj"));
	//resourceManager->AddModel("box",  Model::Load("Resources/box.obj"));
	//resourceManager->AddModel("sphere",    Model::Load("Resources/sphere.obj"));
	//resourceManager->AddModel("cylinder", Model::Load("Resources/cylinder.obj"));
	//resourceManager->AddModel("torus",   Model::Load("Resources/torus.obj"));
	//resourceManager->AddModel("suzanne", Model::Load("Resources/suzanne.obj"));
	//resourceManager->AddModel("skydome", Model::Load("Resources/skydome.obj"));
	resourceManager->AddModel("pbr", Model::Load("Resources/pbr/pbr.gltf"));
	resourceManager->AddModel("box", Model::Load("Resources/Box/Box.gltf"));

	//resourceManager->AddModel("Sponza", Model::Load("Resources/Sponza/sponza.obj"));

	//auto model = Model::Load("Resources/CornellBox/CornellBox-Original.obj");
}

void Test::OnFinalize() {
}
