#pragma once
#include "EditerView.h"

namespace Editer {

    class DebugView :
        public View {
    public:
        using View::View;

        void Render();

    };

}