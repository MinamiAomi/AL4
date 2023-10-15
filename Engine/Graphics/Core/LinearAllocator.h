#pragma once

#include <memory>
#include <queue>

#include "UploadBuffer.h"

class LinearAllocator {
public:
    struct Allocation {
        void* cpu;
        const D3D12_GPU_VIRTUAL_ADDRESS gpu;
    };

    explicit LinearAllocator(size_t pageSize = 2000000);
    
    Allocation Allocate(size_t sizeInByte, size_t alignment = 256);
    void Reset();

    size_t GetPageSize() const { return pageSize_; }

private:
    class Page {
    public:
        Page(size_t sizeInByte);

        bool HasSpace(size_t sizeInByte, size_t alignment);
        Allocation Allocate(size_t sizeInByte, size_t alignment);
        void Reset();

    private:
        UploadBuffer buffer_;
        size_t offset_;
    };

    std::shared_ptr<Page> RequestPage();

    std::deque<std::shared_ptr<Page>> pagePool_;
    std::deque<std::shared_ptr<Page>> availablePages_;

    std::shared_ptr<Page> currentPage_;
    size_t pageSize_;

};