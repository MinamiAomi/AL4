#pragma once
#include"Engine/Scene/BaseScene.h"
#include "Engine/Graphics/ToonModel.h"
#include "Math/Camera.h"

#include<list>

#include"Spike.h"
#include"map.h"
#include"Player.h"

class InGame : public BaseScene {
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnFinalize() override;


	
	/// <summary>
	/// すべての当たり判定処理まとめ
	/// </summary>
	void GetAllCollisions();

	/// <summary>
	/// 死亡判定
	/// </summary>
	void CheckDead();


	
private:
	std::shared_ptr<ToonModel> toonModel_;
	ToonModelInstance modelInstance_;
	Camera camera_;
	
	//マップクラス
	std::unique_ptr<Map>map = nullptr;
	//棘クラス
	std::list<Spike*> spikes;

	//プレイヤークラス
	std::unique_ptr<Player>player_ = nullptr;
};