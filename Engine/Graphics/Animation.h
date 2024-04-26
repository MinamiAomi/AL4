#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Node.h"

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
Vector3 CalculateValue(const AnimationCurve<Vector3>& animationCurve, float time);
Quaternion CalculateValue(const AnimationCurve<Quaternion>& animationCurve, float time);

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

    AnimationSet& GetAnimation(const std::string& name) { return animationSet_.at(name); }

private:
    std::map<std::string, AnimationSet> animationSet_;
};

// ジョイント
struct Joint {
    struct Transform {
        Vector3 translate;
        Quaternion rotate;
        Vector3 scale;
    }transform;
    Matrix4x4 localMatrix;
    Matrix4x4 skeletonSpaceMatrix;
    std::string name;
    std::vector<int32_t> children;
    int32_t index;
    std::optional<int32_t> parent;
};
int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
// スケルトン
struct Skeleton {
    int32_t root;
    std::map<std::string, int32_t> jointMap;
    std::vector<Joint> joints;
    
    void Update();
    void ApplyAnimation(const AnimationSet& animation, float animationTime);
};
Skeleton CreateSkeleton(const Node& rootNode);
