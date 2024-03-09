#pragma once

#include <filesystem>

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/TextureResource.h"

class SphereMapConverter {
public:

    void Initialize(const std::filesystem::path& path);



private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    TextureResource cubeMap_;

};