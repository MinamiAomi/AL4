#pragma once

namespace Editer {

    class EditerManager;
    class View {
    public:
        View(EditerManager& editerManager) : owner(editerManager) {}
        virtual ~View() = 0{}

        EditerManager& owner;
    };

}