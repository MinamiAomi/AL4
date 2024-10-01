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