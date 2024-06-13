#include "Engine.h"

#include <memory>

#include "Game.h"
#include "Graphics/GameWindow.h"
#include "Graphics/RenderManager.h"
#include "Input/Input.h"
#include "Audio/AudioDevice.h"
#include "Scene/SceneManager.h"
#include "ResourceManager.h"
#include "GameObject/GameObjectManager.h"
#ifdef ENABLE_IMGUI
#include "Editer/EditerManager.h"
#endif // ENABLE_IMGUI

namespace {
    Game* g_game = nullptr;
    GameWindow* g_gameWindow = nullptr;
    Graphics* g_graphics = nullptr;
    Input* g_input = nullptr;
    AudioDevice* g_audioDevice = nullptr;
    RenderManager* g_renderManager = nullptr;
    SceneManager* g_sceneManager = nullptr;
    ResourceManager* g_resourceManager = nullptr;
    std::unique_ptr<GameObjectManager> g_gameObjectManager = nullptr;
#ifdef ENABLE_IMGUI
    std::unique_ptr<Editer::EditerManager> g_editerManager = nullptr;
#endif // ENABLE_IMGUI

}

void Engine::Run(Game* game) {


    g_game = game;

    g_gameWindow = GameWindow::GetInstance();
    g_gameWindow->Initialize(L"AL4", 1280, 720);

    g_graphics = Graphics::GetInstance();
    g_graphics->Initialize();

    g_input = Input::GetInstance();
    g_input->Initialize(g_gameWindow->GetHWND());

    g_audioDevice = AudioDevice::GetInstance();
    g_audioDevice->Initialize();

    g_renderManager = RenderManager::GetInstance();
    g_renderManager->Initialize();

    g_sceneManager = SceneManager::GetInstance();
    g_resourceManager = ResourceManager::GetInstance();
    g_gameObjectManager = std::make_unique<GameObjectManager>();
    
#ifdef ENABLE_IMGUI
    g_editerManager = std::make_unique<Editer::EditerManager>();
    g_editerManager->Initialize();
#endif // ENABLE_IMGUI

    g_game->OnInitialize();

    while (g_gameWindow->ProcessMessage()) {
        g_input->Update();
        g_sceneManager->Update();

#ifdef ENABLE_IMGUI
        g_editerManager->Render();
#endif // ENABLE_IMGUI

        g_renderManager->Render();
    }

#ifdef ENABLE_IMGUI
    g_editerManager->Finalize();
#endif // ENABLE_IMGUI
    g_sceneManager->Finalize();
    g_gameObjectManager.reset();
    g_game->OnFinalize();

    g_audioDevice->Finalize();
    g_renderManager->Finalize();
    g_graphics->Finalize();
    g_gameWindow->Shutdown();
}

Game* Engine::GetGame() {
    return g_game;
}

GameWindow* Engine::GetGameWindow() {
    return g_gameWindow;
}

Graphics* Engine::GetGraphics() {
    return g_graphics;
}

Input* Engine::GetInput() {
    return g_input;
}

AudioDevice* Engine::GetAudioDevice() {
    return g_audioDevice;
}

RenderManager* Engine::GetRenderManager() {
    return g_renderManager;
}

SceneManager* Engine::GetSceneManager() {
    return g_sceneManager;
}

ResourceManager* Engine::GetResourceManager() {
    return g_resourceManager;;
}

GameObjectManager* Engine::GetGameObjectManager() {
    return g_gameObjectManager.get();
}

#ifdef ENABLE_IMGUI
Editer::EditerManager* Engine::GetEditerManager() {
    return g_editerManager.get();
}
#endif // ENABLE_IMGUI
