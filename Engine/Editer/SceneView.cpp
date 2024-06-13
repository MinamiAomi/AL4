#include "SceneView.h"

#ifdef ENABLE_IMGUI
#include "Externals/ImGui/imgui.h"
#endif // ENABLE_IMGUI

#ifdef ENABLE_IMGUI
ImVec2 CalcAspectFitSize(const ImVec2& windowSize, const ImVec2& imageSize) {
    float windowAspect = windowSize.x / windowSize.y;
    float imageAspect = imageSize.x / imageSize.y;

    ImVec2 newSize;
    if (windowAspect > imageAspect) {
        // ウィンドウがよりワイドの場合、高さに合わせて幅を計算
        newSize.y = windowSize.y;
        newSize.x = imageSize.x * (windowSize.y / imageSize.y);
    }
    else {
        // ウィンドウがよりタイトの場合、幅に合わせて高さを計算
        newSize.x = windowSize.x;
        newSize.y = imageSize.y * (windowSize.x / imageSize.x);
    }
    return newSize;
}
#endif // ENABLE_IMGUI


namespace Editer {
    void SceneView::Render() {
    }

}