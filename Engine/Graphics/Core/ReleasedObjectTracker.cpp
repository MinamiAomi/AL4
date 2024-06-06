#include "ReleasedObjectTracker.h"

#include <cassert>

void ReleasedObjectTracker::AddObject(Microsoft::WRL::ComPtr<ID3D12Object> releasedObject) {
    assert(releasedObject);
    std::lock_guard<std::mutex> lock(mutex_);
    ReleasedObject object;
    object.ptr = releasedObject;

#ifdef _DEBUG
    wchar_t name[256] = {};
    UINT size = sizeof(name);
    releasedObject->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, name);
    object.name = name;
#endif // _DEBUG

    auto& back = trackingObjectLists_.back();
    back.emplace_back(std::move(object));
}

void ReleasedObjectTracker::FrameIncrementForRelease() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (size_t i = 1; i < trackingObjectLists_.size(); ++i) {
        std::swap(trackingObjectLists_[i - 1], trackingObjectLists_[i]);
    }
    trackingObjectLists_.back().clear();
}

void ReleasedObjectTracker::AllRelease() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& trackingObjecetList : trackingObjectLists_) {
        trackingObjecetList.clear();
    }
}
