#pragma once
#include "EditerView.h"

namespace Editer {

    class ProjectView : 
        public View {
    public:
        using View::View;

        void Render();

    private:
        float leftWindowSize_ = 100.0f;
    };

}