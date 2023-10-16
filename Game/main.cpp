#include <Windows.h>

#include "Engine/Engine.h"
#include "OneButton.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    Game* game = new OneButton;
    Engine::Run(game);
    delete game;

    return 0;
}