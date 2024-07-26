#include "Asset.h"

#include <cassert>
#include <thread>

#include "ThreadPool.h"
#include "Engine.h"


#include "Graphics/Texture.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Graphics/Animation.h"
#include "Audio/Sound.h"

void Asset::Load(const std::filesystem::path& path, const std::string& name) {
    assert(!path.empty());

    path_ = path;
    // 名前が指定されていない場合はパスの拡張子を除いた名前を使用
    name_ = name.empty() ? path.stem().string() : name;

    // 非同期読み込み
    Engine::GetThreadPool()->PushTask([this]() {
        state_ = State::Loading;
        InternalLoad();
        state_ = State::Loaded;
        });
}

void TextureAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Texture;
    core_ = Texture::Load(path_);
}

void ModelAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Model;
    core_ = Model::Load(path_);
}

void MaterialAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Material;
    core_ = std::shared_ptr<Material>();
}

void AnimationAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Animation;
    core_ = Animation::Load(path_);
}

void SoundAsset::InternalLoad() {
    assert(state_ == State::Loading);
    type_ = Type::Sound;
    core_ = Sound::Load(path_);
}
