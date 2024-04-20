#include "Engine.h"

#include "Game.h"

#include "Graphics/GameWindow.h"
#include "Graphics/RenderManager.h"
#include "Input/Input.h"
#include "Audio/AudioDevice.h"
#include "Scene/SceneManager.h"

void Engine::Run(Game* game) {
    auto gameWindow = GameWindow::GetInstance();
    gameWindow->Initialize(L"AL4", 1280, 720);

    auto graphics = Graphics::GetInstance();
    graphics->Initialize();

    auto input = Input::GetInstance();
    input->Initialize(gameWindow->GetHWND());

    auto audio = AudioDevice::GetInstance();
    audio->Initialize();

    auto renderManager = RenderManager::GetInstance();
    renderManager->Initialize();
    
    auto sceneManager = SceneManager::GetInstance();

    game->OnInitialize();

    while (gameWindow->ProcessMessage()) {
        input->Update();
        sceneManager->Update();

        renderManager->Render();
    }

    game->OnFinalize();

    audio->Finalize();
    renderManager->Finalize();
    graphics->Finalize();
    gameWindow->Shutdown();
}
