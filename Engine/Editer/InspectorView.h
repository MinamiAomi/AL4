///
/// インスペクタービュー
/// 

#pragma once
#include "EditerView.h"

#include <memory>

#include "EditerInterface.h"

namespace LIEngine {

    namespace Editer {

        class InspectorView :
            public View {
        public:
            using View::View;

            void Render(const std::shared_ptr<SelectableInEditer>& selectableInEditer);

        };

    }

}