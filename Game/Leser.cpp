#include "Leser.h"

Leser::Leser() {}
Leser::~Leser() {}

void Leser::Initalize(std::shared_ptr<ToonModel> model, const Transform& PlayerworldTransform_) {
	leserModel_ = model;
	worldTransform_.scale.x = 0.5f;
	worldTransform_.scale.y = 1.5f;
	worldTransform_.scale.z = 0.5f;
	worldTransform_.translate = PlayerworldTransform_.translate;
}

void Leser::Update() {
	worldTransform_.translate.y -= 4.0f;
	worldTransform_.UpdateMatrix();
	AliveCount--;
	if (AliveCount <= 0) {
		IsAlive = false;
	}

}
//void Leser::Draw() {}
void Leser::OnCollision() {
	IsAlive = false;

}