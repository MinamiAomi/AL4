#include "Engine.h"

#include "Application/Game.h"

#include "Graphics/GameWindow.h"
#include "Graphics/RenderManager.h"
#include "Input/Input.h"
#include "Audio/Audio.h"
#include "Scene/SceneManager.h"

void Engine::Run(Game* game) {
    auto gameWindow = GameWindow::GetInstance();
    gameWindow->Initialize(L"OneButton", 1280, 720);

    auto input = Input::GetInstance();
    input->Initialize(gameWindow->GetHWND());

    auto audio = Audio::GetInstance();
    audio->Initialize();

    auto renderManager = RenderManager::GetInstance();
    renderManager->Initialize();
    
    auto sceneManager = SceneManager::GetInstance();

    game->OnInitialize();

    while (gameWindow->ProcessMessage()) {
        input->Update();
        audio->Update();
        sceneManager->Update();

        renderManager->Render();
    }

    game->OnFinalize();

    renderManager->Finalize();
    gameWindow->Shutdown();
}
