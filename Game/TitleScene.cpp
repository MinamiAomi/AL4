#include "TitleScene.h"

#include "Engine/Graphics/ModelLoader.h"
#include "Engine/Graphics/RenderManager.h"
#include "Engine/Scene/SceneManager.h"

void TitleScene::OnInitialize()
{
	toonModel_ = std::make_shared<ToonModel>();
	toonModel_->Create(ModelData::LoadObjFile("Resources/Model/sphere.obj"));

	modelInstance_.SetModel(toonModel_);

	RenderManager::GetInstance()->SetCamera(camera_);

	
}

void TitleScene::OnUpdate()
{
	transform_.UpdateMatrix();
	modelInstance_.SetWorldMatrix(transform_.worldMatrix);
	camera_.UpdateMatrices();

}

void TitleScene::OnFinalize()
{

}
