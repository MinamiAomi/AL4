#include <Windows.h>

#include "Engine/Engine.h"
#include "OneButton.h"

#include "Engine/Graphics/GameWindow.h"
#include "Engine/Graphics/RenderManager.h"
#include "Engine/Scene/SceneManager.h"

#include "Engine/Graphics/ModelLoader.h"
#include "Engine/Graphics/ToonModel.h"

#include "Engine/Math/Camera.h"

#include "Graphics/ImGuiManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    Game* game = new OneButton;
    Engine::Run(game);
    delete game;


    //auto gameWindow = GameWindow::GetInstance();
    //gameWindow->Initialize(L"OneButton", 1280, 720);

    //auto renderManager = RenderManager::GetInstance();
    //renderManager->Initialize();

    //auto sceneManager = SceneManager::GetInstance();
    //sceneManager->ChangeScene();

    //{
    //    ModelData modelData = ModelData::LoadObjFile("Resources/Model/teapot.obj");
    //    std::shared_ptr<ToonModel> toonModel = std::make_shared<ToonModel>();
    //    toonModel->Create(modelData);
    //    ToonModelInstance toonModelInstance;
    //    toonModelInstance.SetModel(toonModel);
    //  //  toonModelInstance.SetUseOutline(false);

    //    Camera camera;
    //    renderManager->SetCamera(camera);

    //    Vector3 cameraPos = camera.GetPosition();
    //    Vector3 cameraRot = {};

    //        while (gameWindow->ProcessMessage()) {

    //            ImGui::Begin("Camera");
    //            ImGui::DragFloat3("Position", &cameraPos.x, 0.01f);
    //            ImGui::DragFloat3("Rotate", &cameraRot.x, 1.0f, -360.0f, 360.0f);
    //            ImGui::End();

    //            camera.SetPosition(cameraPos);
    //            camera.SetRotate(Quaternion::MakeFromEulerAngle(cameraRot * Math::ToRadian));

    //            camera.UpdateMatrices();

    //            renderManager->Render();
    //        }
    //}


    //renderManager->Finalize();
    //gameWindow->Shutdown();

    return 0;
}