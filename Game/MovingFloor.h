#pragma once
#include "Floor.h"

class MovingFloor :
    public Floor {
public:
    void Initialize() override;
    void Update() override;

    void SetMovement(float movement) { movement_ = movement; }

private:
    float movement_ = 5.0f;
    float moveParam_ = 0;

};