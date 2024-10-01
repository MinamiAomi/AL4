#include "ModelAsset.h"

#include <cassert>

#include "Graphics/Model.h"

void ModelAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Model;
    core_ = Model::Load(path_);
}