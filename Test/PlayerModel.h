#include <functional>
#include <optional>
#include <array>

#include "Math/Transform.h"
#include "Math/AnimationNode.h"
#include "Graphics/Model.h"

class PlayerModel {
public:
    enum Part {
        kBody,
        kHead,
        kRightArm,
        kLeftArm,
        kRightFoot,
        kLeftFoot,
        kBag,

        kNumParts
    };

    enum AnimationType {
        kWait,
        kWalk,

        kNumAnimationTypes
    };

    void Initialize(Transform* transform);
    void Update();

    void PlayAnimation(AnimationType animation, bool isLoop);
    void StopAnimation();
    bool IsPlayingAnimation() { return animation_.has_value(); }
    const std::optional<AnimationType>& GetAnimationType() const { return animation_; }

private:
    struct TransformParameter {
        Vector3 scale;
        Quaternion rotate;
        Vector3 translate;
    };
    using AnimationData = std::array<TransformParameter, kNumParts>;

    static const Vector3 kInitialTranslates[kNumParts];
    static std::function<void(PlayerModel&)> kAnimationStepFuncTable[kNumAnimationTypes];
    static std::function<AnimationData(PlayerModel&, float)> kAnimationTable[kNumAnimationTypes];
    static Animation::FloatNode kWalkHeightAnimation;
    static std::array<Animation::QuaternionNode, kNumParts> kWalkRotateAnimationTable;

    void WaitAnimationStep();
    void WalkAnimationStep();

    AnimationData WaitAnimation(float animationTime);
    AnimationData WalkAnimation(float animationTime);

    // 続行する場合true
    bool UpdateAnimationParameter(float delta);

    const Transform* baseTransform_;

    Transform partTransform_[kNumParts];
    std::shared_ptr<ModelInstance> partModels_[kNumParts];

    float animationParameter_;
    std::optional<AnimationType> animation_;

    bool isLoop_;
};