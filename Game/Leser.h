#pragma once
#pragma once

//kamataEngine
#include "Engine/Graphics/ToonModel.h"
#include "Engine/Math/Transform.h"

class Leser {
public:
	Leser();
	~Leser();

	void Initalize(std::shared_ptr<ToonModel> model, const Transform& PlayerworldTransform_);
	void Update();
	//void Draw();
	void OnCollision();
	bool GetIsAlive() { return IsAlive; }

private:

	//kamataEngine
	std::shared_ptr<ToonModel> leserModel_ = nullptr;
	Transform worldTransform_;

	//
	int AliveCount = 240;
	bool IsAlive = true;
};
