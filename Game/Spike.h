#pragma once

#include "Engine/Graphics/ToonModel.h"
#include "Math/Transform.h"


class Spike {
public:


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="world">ワールドT</param>
	/// <param name="model">モデル</param>
	/// <param name="State">状態</param>
	/// <param name="velo">移動ベクトル</param>
	void Initialize(int num,Transform world, std::shared_ptr<ToonModel> toonModel, int State, Vector3 velo = { 0,0,0 });


	void Update();

	

#pragma region ゲッター
	Transform GetWorld() const { return world_; }
	const Vector3 GetmatWtranstate() const {
		return {
			world_.worldMatrix.m[3][0],
			world_.worldMatrix.m[3][1],
			world_.worldMatrix.m[3][2],
		};
	}

	const float GetWide() { return wide_; }

	Vector3 GetVelocity_() const { return velocity_; }

	bool IsDead() const { return isDead_; }

	const int GetIdentificationNum() { return spikeNum_; }
#pragma endregion

#pragma region オンコリ
	//プレイヤーと当たった時
	void OnCollisionPlayer();
	//ボスと当たった時
	void OnCollisionBoss();
	//プレイヤーのレーザー攻撃に当たった時
	void OnCollisionPlayerBeam();
	//プレイヤーの攻撃の爆破に当たった時
	void OnCollisionPlayerExplosion(Vector3 ExpPos);

	//棘同士当たった時
	void OnCollisionSpike();

	//プレイヤーのため攻撃に当たった時
	void OnCollisionPlayerStump();

	void OnCollisionWall();
#pragma endregion




private:
	/// <summary>
	/// 針の状態
	/// </summary>
	enum State {
		// 木についている
		Stay,
		// 落ちる
		Falling,
		// 埋まる
		FillUp,
		// BAKUHATU★
		Explosion,
		//None
		None
	};

	int spikeNum_ = 0;

	//ワールド
	Transform world_;
	//モデル
	std::shared_ptr<ToonModel> toonModel_;

	ToonModelInstance modelInstance_;

	//仮
	float wide_;

	//状態
	State state_;

	//移動量
	Vector3 velocity_;

	//減速量
	const float gensoku = 1.0f / 100.0f;

	//死亡判定
	bool isDead_ = false;

	//爆破アニメーションのカウント
	int animationCount_ = 0;

	//アニメーションの終わりカウント
	const int maxAnimationCount = 60;


};
