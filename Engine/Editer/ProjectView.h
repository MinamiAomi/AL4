#pragma once
#include "EditerView.h"

namespace Editer {

    class ProjectView : 
        public View {
    public:
        using View::View;

        void Render();

    private:
        void RenderLeftWindow();
        void RenderRightWindow();

        float leftWindowSize_ = 100.0f;
        float itemScale_ = 1.0f;
    };

}