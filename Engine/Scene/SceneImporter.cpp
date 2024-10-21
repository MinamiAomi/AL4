#include "SceneImporter.h"

#include <cassert>
#include <fstream>

#include "Externals/nlohmann/json.hpp"

void SceneInporter::Inport(const std::filesystem::path& path) {

    // ファイルを読み込む
    std::ifstream file(path);
    assert(file.is_open());

    nlohmann::json json;
    file >> json;

    file.close();

   
}
