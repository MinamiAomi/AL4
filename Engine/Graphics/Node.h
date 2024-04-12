#pragma once

#include <string>
#include <vector>

#include "Math/MathUtils.h"

struct Node {
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<Node> children;
};

