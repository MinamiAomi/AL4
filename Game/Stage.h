#pragma once

#include <memory>
#include <vector>

#include "Floor.h"

class Stage {
public:
    void Initialize();
    void Update();

private:

    std::vector<std::shared_ptr<Floor>> floors_;
};