#pragma once
#include "Engine/Game.h"

class AL4Game :
    public Game {
public:

    void OnInitialize() override;
    void OnFinalize() override;

private:

};