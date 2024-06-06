#include "SceneObject.h"

#include "Framework/ResourceManager.h"

#include "Graphics/ImGuiManager.h"

void SceneObject::Initialize() {
    model_ = std::make_unique<ModelInstance>();

    transform.translate = Vector3::zero;
    transform.rotate = Quaternion::identity;
    transform.scale = Vector3::one;
}

