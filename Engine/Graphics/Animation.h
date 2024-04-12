#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "Math/MathUtils.h"
#include "Math/Transform.h"

// キーフレーム
template<typename T>
struct Keyframe {
    float time;
    T value;
};
// アニメーションカーブ
template<typename T>
struct AnimationCurve {
    std::vector<Keyframe<T>> keyframes;
};
// ノードアニメーション
struct NodeAnimation {
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};
// アニメーション
struct AnimationSet {
    float duration;
    std::map<std::string, NodeAnimation> nodeAnimations;
};

class Animation {
public:
    static std::shared_ptr<Animation> Load(const std::filesystem::path& path);

private:
    std::vector<AnimationSet> animationSet_;
};

// ジョイント
struct Joint {
    Transform transform;
    Matrix4x4 localMatrix;
    Matrix4x4 skeletonSpaceMatrix;
    std::string name;
    std::vector<int32_t> children;
    int32_t index;
    std::optional<int32_t> parent;
};
// スケルトン
struct Skeleton {
    int32_t root;
    std::map<std::string, int32_t> jointMap;
    std::vector<Joint> joints;
};