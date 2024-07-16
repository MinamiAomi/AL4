#pragma once

#include <string>
#include <filesystem>

class Resource {
public:
    enum Type {
        Texture,
        Model,
        Material,
        Animation,
        Sound


    };
    
    const std::filesystem::path& GetPath() const { return path_; }
    const std::string& GetName() const { return name_; }

protected:
    std::filesystem::path path_;
    std::string name_;

};