#include "ReleasedObjectTracker.h"

void ReleasedObjectTracker::AddObject(Microsoft::WRL::ComPtr<ID3D12Object> releasedObject) {
    trackingObjectLists_.back().emplace_back(releasedObject);
}

void ReleasedObjectTracker::FrameIncrementForRelease() {
    for (size_t i = 1; i < trackingObjectLists_.size(); ++i) {
        trackingObjectLists_[i - 1] = std::move(trackingObjectLists_[i]);
    }
}

void ReleasedObjectTracker::AllRelease() {
    for (auto& trackingObjecetList : trackingObjectLists_) {
        trackingObjecetList.clear();
    }
}
