#include "LinearAllocator.h"

#include <assert.h>

#include "Helper.h"

LinearAllocator::LinearAllocator(size_t pageSize) : pageSize_(pageSize) {}

LinearAllocator::Allocation LinearAllocator::Allocate(size_t sizeInByte, size_t alignment) {
    assert(sizeInByte <= pageSize_);

    // 現在のページがない、または、ページに空きがない場合
    if (!currentPage_ || !currentPage_->HasSpace(sizeInByte, alignment)) {
        // 新しいページを要求
        currentPage_ = RequestPage();
    }
    // 割り当て
    return currentPage_->Allocate(sizeInByte, alignment);
}

void LinearAllocator::Reset() {
    currentPage_ = nullptr;
    availablePages_ = pagePool_;

    for (auto& page : availablePages_) {
        page->Reset();
    }
}

std::shared_ptr<LinearAllocator::Page> LinearAllocator::RequestPage() {
    std::shared_ptr<Page> page;

    // 利用可能なページがある
    if (!availablePages_.empty()) {
        page = availablePages_.front();
        availablePages_.pop_front();
    }
    // 利用可能なページがない
    else {
        // 新しく生成する
        page = std::make_shared<Page>(pageSize_);
        pagePool_.push_back(page);
    }

    return page;
}

LinearAllocator::Page::Page(size_t sizeInByte) :
    offset_(0) {
    buffer_.Create(L"LinearAllocator Page", sizeInByte);
}

bool LinearAllocator::Page::HasSpace(size_t sizeInByte, size_t alignment) {
    size_t alignedSize = Helper::AlignUp(sizeInByte, alignment);
    size_t alignedOffset = Helper::AlignUp(offset_, alignment);

    return alignedOffset + alignedSize <= buffer_.GetBufferSize();
}

LinearAllocator::Allocation LinearAllocator::Page::Allocate(size_t sizeInByte, size_t alignment) {
    size_t alignedSize = Helper::AlignUp(sizeInByte, alignment);
    offset_ = Helper::AlignUp(offset_, alignment);

    Allocation allocation{
        .cpu = static_cast<uint8_t*>(buffer_.GetCPUData()) + offset_,
        .gpu = buffer_.GetGPUVirtualAddress() + offset_
    };
    offset_ += alignedSize;
    return allocation;
}

void LinearAllocator::Page::Reset() {
    offset_ = 0;
}
