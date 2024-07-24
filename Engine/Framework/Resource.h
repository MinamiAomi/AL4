#pragma once

#include <string>
#include <filesystem>

#include "Editer/EditerInterface.h"

class Resource :
    public Editer::SelectableInEditer {
public:
    enum Type {
        Texture,
        Model,
        Material,
        Animation,
        Sound,

        NumTypes
    };
    
    enum State {
        Unloaded,
        Loading,
        Loaded,

        NumStates,
    };

    virtual ~Resource() {}
    virtual void RenderInInspectorView() = 0;

    void SetName(const std::string& name) { name_ = name; }

    const std::filesystem::path& GetPath() const { return path_; }
    const std::string& GetName() const { return name_; }
    Type GetType() const { return type_; }

protected:
    std::filesystem::path path_;
    std::string name_;
    Type type_;
    State state_;
};