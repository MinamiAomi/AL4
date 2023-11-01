#pragma once

#include <memory>

class Game;

class Engine {
public:
    static void Run(Game* game);
};