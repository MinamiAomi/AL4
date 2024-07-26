#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include "Editer/EditerInterface.h"

class Asset :
    public Editer::SelectableInEditer {
public:
    enum class Type {
        None,
        Texture,
        Model,
        Material,
        Animation,
        Sound,

        NumTypes
    };

    enum class State {
        Unloaded,
        Loading,
        Loaded,

        NumStates,
    };

    virtual ~Asset() {}
    void Load(const std::filesystem::path& path, const std::string& name = "");

    virtual void RenderInInspectorView() override = 0;

    void SetName(const std::string& name) { name_ = name; }

    const std::filesystem::path& GetPath() const { return path_; }
    const std::string& GetName() const { return name_; }
    Type GetType() const { return type_; }

protected:
    virtual void InternalLoad() = 0;

    std::filesystem::path path_;
    std::string name_;
    Type type_ = Type::None;
    State state_ = State::Unloaded;
};

class Texture;
class Model;
class Material;
class Animation;
class Sound;

class TextureAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Texture> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Texture> core_;
};

class ModelAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Model> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Model> core_;
};

class MaterialAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Material> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Material> core_;
};

class AnimationAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Animation> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Animation> core_;
};

class SoundAsset :
    public Asset {
public:
    void RenderInInspectorView() override {}

    std::shared_ptr<Sound> Get() const { return core_; }

private:
    void InternalLoad() override;

    std::shared_ptr<Sound> core_;
};