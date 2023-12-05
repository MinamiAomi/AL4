#pragma once
#include "../Core/UploadBuffer.h"

#include <vector>

class ShaderRecord {
public:


private:
    using Data = std::pair<void*, size_t>;
    
    Data idenfilter;
};

class ShaderTable :
    public GPUResource {
public:

private:
    std::vector<ShaderRecord> shaderRecords_;
};