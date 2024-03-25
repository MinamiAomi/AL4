#include "Player.h"

#include "Input/Input.h"
#include "FollowCamera.h"
#include "Graphics/ResourceManager.h"

void Player::Initialize() {

    //model_ = std::make_unique<ModelInstance>();
    //model_->SetModel(ResourceManager::GetInstance()->FindModel("Player"));
    //model_->SetIsActive(true);


    transform.translate = Vector3::zero;
    transform.rotate = Quaternion::identity;
    transform.scale = Vector3::one;

    playerModel_.Initialize(&transform);
    playerModel_.PlayAnimation(PlayerModel::kWait, true);
}

void Player::Update() {

    auto input = Input::GetInstance();
    auto followCamera = followCamera_.lock();

    Vector3 move{};
    // Gamepad入力
    auto& xinputState = input->GetXInputState();
    const float margin = 0.8f;
    const float shortMaxReci = 1.0f / float(SHRT_MAX);
    move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
    move *= shortMaxReci;
    if (move.Length() < margin) {
        move = Vector3::zero;
    }

    // 移動処理
    if (move != Vector3::zero) {
        move = move.Normalized();
        // 地面に水平なカメラの回転
        if (followCamera) {
            move = followCamera->GetCamera()->GetRotate() * move;
        }
        move.y = 0.0f;
        move = move.Normalized() * 0.1f;

        // 親がいる場合親の空間にする
        const Transform* parent = transform.GetParent();
        if (parent) {
            move = parent->worldMatrix.Inverse().ApplyRotation(move);
        }

        // 移動
        transform.translate += move;
        // 回転
        //transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation(move));

        move = transform.rotate.Conjugate() * move;
        Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
        transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;

        if (playerModel_.GetAnimationType() != PlayerModel::AnimationType::kWalk) {
            playerModel_.PlayAnimation(PlayerModel::kWalk, true);
        }
    }
    else {
        if (playerModel_.GetAnimationType() != PlayerModel::AnimationType::kWait) {
            playerModel_.PlayAnimation(PlayerModel::kWait, true);
        }
    }


    transform.UpdateMatrix();
    playerModel_.Update();
    //model_->SetWorldMatrix(transform.worldMatrix);
}
