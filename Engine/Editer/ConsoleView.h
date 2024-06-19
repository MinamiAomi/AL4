#pragma once
#include "EditerView.h"

namespace Editer {

    class ConsoleView :
        public View {
    public:
        using View::View;

        void Render();

    };

}