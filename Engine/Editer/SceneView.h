#pragma once
#include "EditerView.h"

class CommandContext;

namespace Editer {

    class SceneView :
        public View {
    public:
        using View::View;

        void Render(CommandContext& commandContext);

    };

}