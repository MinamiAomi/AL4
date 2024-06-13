#pragma once

#include <memory>

#include "Input/Input.h"


class Game;
class GameWindow;
class Graphics;
class Input;
class AudioDevice;
class RenderManager;
class SceneManager;
class GameObjectManager;
class ResourceManager;

#ifdef ENABLE_IMGUI
namespace Editer {
    class EditerManager;
}
#endif // ENABLE_IMGUI


class Engine {
public:
    static void Run(Game* game);

    static Game* GetGame();
    static GameWindow* GetGameWindow();
    static Graphics* GetGraphics();
    static Input* GetInput();
    static AudioDevice* GetAudioDevice();
    static RenderManager* GetRenderManager();
    static SceneManager* GetSceneManager();
    static ResourceManager* GetResourceManager();
    static GameObjectManager* GetGameObjectManager();
    static Editer::EditerManager* GetEditerManager();
};