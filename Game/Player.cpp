#include "Player.h"

Player::Player() {}
Player::~Player() {}

void Player::Initalize(const Vector3& position) {
	//レーザーのモデル
	/*leser_model = new Model();
	leser_model = Model::Create();*/
	//プレイヤーのモデル
	worldTransform_.translate = position;

	input = Input::GetInstance();
}

void Player::Update() {

	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitalize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitalize();
			break;
		case Behavior::kDrop:
			BehaviorDropInitalize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	case Behavior::kDrop:
		BehaviorDropUpdate();
		break;
	}
	//TODO
	/*for (Leser* leser : lesers_) {
		leser->Update();
	}*/

	worldTransform_.UpdateMatrix();
}

//void Player::Draw() {
//	for (Leser* leser : lesers_) {
//		leser->Draw(ViewProjection_);
//	}
//}

void Player::OnCollision() {
	if (DropFlag) {
		return;
	}
	else {
		HP -= 1;
	}

}

void Player::BehaviorRootInitalize() {
	behavior_ = Behavior::kRoot;
	DropFlag = false;
}

void Player::BehaviorRootUpdate() {
	worldTransform_.translate.y -= gravity;
	worldTransform_.translate.x += moveXaxisSpeed;
	//TODO
	/*lesers_.remove_if([](Leser* leser) {
		if (leser->GetIsAlive()) {
			delete leser;
			return true;
		}
		return false;
		});*/
	//スペースを押すとジャンプする
	if (input->GetInstance()->IsKeyTrigger(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
	}
}

void Player::BehaviorJumpInitalize() {
	behavior_ = Behavior::kJump;
	Jumpforce = 2.0f;
	moveXaxisSpeed *= -1;
	//TODO : Leserクラスを作成
	//Leser* leser_ = new Leser();
	//leser_->Initalize(leser_model, worldTransform_);
	//lesers_.push_back(leser_);
	DropCount = 0;
	DropFlag = false;
}

void Player::BehaviorJumpUpdate() {

	worldTransform_.translate.y += Jumpforce;
	worldTransform_.translate.x += moveXaxisSpeed;
	if (Jumpforce > 0) {
		Jumpforce -= gravity;
	}
	else if (Jumpforce <= 0) {
		Jumpforce = -kXaxisSpeed;
	}

	DropCount++;
	if (input->IsKeyTrigger(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
	}
	else if (
		DropCount == kDropAnime && input->IsKeyPressed(DIK_SPACE) != 0) {
		behaviorRequest_ = Behavior::kDrop;
	}
}

void Player::BehaviorDropInitalize() {
	behavior_ = Behavior::kDrop;
}

void Player::BehaviorDropUpdate() {
	worldTransform_.translate.y += 0.03f;
	DropCount++;
	if (DropCount >= kDropAnime * 3) {
		Attack();
	}
	if (input->IsKeyRelease(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorHitInitalize() {
	behavior_ = Behavior::kHit;
	//t = 0.0f;
}

void Player::BehaviorHitUpdate() {
	//t += 0.1f;
	//TODO : 敵に当たった時にLerpで画面上まで移動
	//worldTransform_.translation_.y = Lerp();
	
	/*if (t <= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}*/
}

void Player::Attack() {
	worldTransform_.translate.y -= 2.0f;
	DropFlag = true;
}



