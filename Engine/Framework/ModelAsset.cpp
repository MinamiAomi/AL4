#include "ModelAsset.h"

#include <cassert>

#include "Graphics/Model.h"
#include "Graphics/ImGuiManager.h"

void ModelAsset::RenderInInspectorView() {
#ifdef ENABLE_IMGUI
    Asset::RenderInInspectorView();
    if (core_) {
        ImGui::Text("Num Vertices  : %d", core_->GetNumVertices());
        ImGui::Text("Num Indices   : %d", core_->GetNumIndices());
        ImGui::Text("Num Meshes    : %d", core_->GetMeshes().size());
        ImGui::Text("Num Materials : %d", core_->GetMaterials().size());
    }
#endif // ENABLE_IMGUI
}

void ModelAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Model;
    core_ = Model::Load(path_);
}