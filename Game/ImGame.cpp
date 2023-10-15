#include"ImGame.h"

#include"Externals/ImGui/imgui.h"
#include "Engine/Graphics/RenderManager.h"
#include"Engine/Graphics/ModelLoader.h"
#include "Engine/Scene/SceneManager.h"


#include "Math/Transform.h"

void InGame::OnInitialize()
{

	RenderManager::GetInstance()->SetCamera(camera_);

	//カメラ座標初期化
	Vector3 camerapos = { 0.0f,0.0f,-50.0f };
	camera_.SetPosition(camerapos);



	toonModel_ = std::make_shared<ToonModel>();
	toonModel_->Create(ModelData::LoadObjFile("Resources/Model/sphere.obj"));

	
	//std::vector<std::shared_ptr<ToonModel>> modelMap = { toonModel_ };

	//マップクラス初期化
	map = std::make_unique<Map>();
	map->Initialize();

	//スパイクのTransformコピー
	std::vector<Transform> spikeWorld = map->GetSpikeWorld();
	//棘の数取得
	int spileNum = (int)spikeWorld.size();
	//棘の初期設定
	for (int num = 0; num < spileNum; num++) {		
		int sizeNum=(int)spikes.size();
		Spike* spike_ = new Spike();
		spike_->Initialize(sizeNum,spikeWorld[num], toonModel_,0);
		spikes.push_back(spike_);
	}

	player_ = std::make_unique<Player>();
	player_->Initalize(map->GetPlayerPosition().translate);
}

void InGame::OnUpdate()
{
	
	

	//マップ更新
	map->Update();

	//棘更新
	for (Spike* spike : spikes) {
		spike->Update();
	}

	//カメラ更新
	camera_.UpdateMatrices();

	player_->Update();

	GetAllCollisions();
	CheckDead();

}


bool CheckHitSphere(Vector3 p1, float w1, Vector3 p2, float w2) {

	Vector3 p = p1 - p2;

	float Length = p.x * p.x + p.y * p.y + p.z * p.z;

	if (Length <= w1 + w2) {
		return true;
	}
	else {
		return false;
	}
}



void InGame::GetAllCollisions() {
	//スパイクのWorld
	for (Spike* spike : spikes) {
		//座標と半径取得
		Vector3 SPIKE= spike->GetmatWtranstate();
		float S_wide = spike->GetWide();

		//プレイヤー座標と半径
		Vector3 PLAYER = player_->GetmatWtranslate();
		float P_wide = player_->GetWide();


#pragma region プレイヤー
		//当たった時の処理
		if (CheckHitSphere(SPIKE, S_wide, PLAYER, P_wide)) {
			spike->OnCollisionPlayer();
			player_->OnCollision();
		}
#pragma endregion

#pragma region プレイヤービーム

#pragma endregion

#pragma region プレイヤービーム爆風

#pragma endregion

#pragma region BOSS

#pragma endregion

#pragma region 壁
		if (map->IsHitWall(SPIKE, S_wide)) {
			spike->OnCollisionWall();
		}
#pragma endregion

#pragma region 棘同士
		for (Spike* spike2 : spikes) {
			Vector3 SPIKE2 = spike2->GetmatWtranstate();
			float S2_wide = spike->GetWide();

			//座標が同じ(同じもの)でないことを確認
			if (spike->GetIdentificationNum() != spike2->GetIdentificationNum()) {

				if (CheckHitSphere(SPIKE, S_wide, SPIKE2, S2_wide)) {
					
					//二つの円の間の距離取得
					Vector3 leng = (SPIKE - SPIKE2);
					leng /= 2;

					//新しい円の半径設定
					float newSize = (S_wide + S2_wide);
					
					//新しいTransform作成
					Transform newSpike;
					newSpike.translate = leng;							//位置設定
					newSpike.scale = { newSize,newSize ,newSize };		//サイズ設定

					//サイズ取得
					int sizeNum = (int)spikes.size();

					//新しいスパイクの生成
					Spike* newspike = new Spike();
					newspike->Initialize(sizeNum, newSpike, toonModel_, 0);
					
					//ぷっす
					spikes.push_back(newspike);

					//くっついた二つをデリーと
					delete spike;
					delete spike2;


					spike = nullptr;
					spike2 = nullptr;

				}

			}


		}
#pragma endregion



	}
}

void InGame::CheckDead() {
	spikes.remove_if([](Spike* spike) {
		if (spike->IsDead()) {
			delete spike;
			return true;
		}
		return false;
		});
}


void InGame::OnFinalize()
{
	for (Spike* spike : spikes) {
		delete spike;
		spike = nullptr;
	}

}