#pragma once
#include "EditerView.h"

namespace Editer {

    class SceneView :
        public View {
    public:
        using View::View;

        void Render();

    };

}