#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"

void GameScene::OnInitialize() {
    camera_ = std::make_shared<Camera>();
    camera_->SetPosition(camera_->GetPosition() + Vector3{0.0f, 3.0f, -2.0f});
    camera_->UpdateMatrices();
    RenderManager::GetInstance()->SetCamera(camera_);

    floor_ = Model::Load("Resources/Floor/Floor.obj");
    teapot_ = Model::Load("Resources/Teapot/teapot.obj");
    bunny_ = Model::Load("Resources/Bunny/bunny.obj");

    floorModel_.SetModel(floor_);
    teapotModel_.SetModel(teapot_);
    teapotModel_.SetWorldMatrix(Matrix4x4::MakeTranslation({ 0.0f, 1.0f, 0.0f }));
    bunnyModel_.SetModel(bunny_);
    bunnyModel_.SetWorldMatrix(Matrix4x4::MakeTranslation({ 0.0f, 3.0f, 0.0f }));
}

void GameScene::OnUpdate() {

	Input* input = Input::GetInstance();

	auto mouseMoveX = input->GetMouseMoveX();
	auto mouseMoveY = input->GetMouseMoveY();
	auto wheel = input->GetMouseWheel();

	Quaternion rotate = camera_->GetRotate();
	Vector3 position = camera_->GetPosition();
	
	Vector3 diffPosition;

	if (input->IsMousePressed(1)) {
		constexpr float rotSpeed = Math::ToRadian * 0.1f;
		rotate = rotate * Quaternion::MakeFromEulerAngle({ rotSpeed * static_cast<float>(mouseMoveY),rotSpeed * static_cast<float>(mouseMoveX),0.0f });
	}
	else if (input->IsMousePressed(2)) {
		Vector3 cameraX = rotate.GetRight() * (-static_cast<float>(mouseMoveX) * 0.01f);
		Vector3 cameraY = rotate.GetUp() * (static_cast<float>(mouseMoveY) * 0.01f);
		diffPosition += cameraX + cameraY;
	}
	else if (wheel != 0) {
		Vector3 cameraZ = rotate.GetForward() * (static_cast<float>(wheel / 120) * 0.5f);
		diffPosition += cameraZ;
	}

	camera_->SetPosition(position + diffPosition);
	camera_->SetRotate(rotate );
	camera_->UpdateMatrices();
}

void GameScene::OnFinalize() {
}
