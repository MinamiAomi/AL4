#pragma once

#include <functional>

#include "Math/MathUtils.h"
#include "Math/Geometry.h"

class Collider;
class SphereCollider;
class BoxCollider;

struct CollisionInfo {
    Collider* collider;
    Vector3 normal;
    float depth;
};

class Collider {
    friend class CollisionManager;
public:
    using Callback = std::function<void(const CollisionInfo&)>;

    Collider();
    virtual ~Collider();

    virtual bool IsCollision(Collider* collider, CollisionInfo& collisionInfo) = 0;
    virtual bool IsCollision(SphereCollider* collider, CollisionInfo& collisionInfo) = 0;
    virtual bool IsCollision(BoxCollider* collider, CollisionInfo& collisionInfo) = 0;

    void SetOnCollision(Callback callback) { onCollision_ = callback; }
    void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }
    void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }

protected:
    bool CanCollision(Collider* other) const;

    Callback onCollision_;
    uint32_t collisionAttribute_ = 0xFFFFFFFF;
    uint32_t collisionMask_ = 0xFFFFFFFF;
    bool isActive_;
};

class SphereCollider :
    public Collider {
    friend class BoxCollider;
public:
    bool IsCollision(Collider* collider, CollisionInfo& collisionInfo) override;
    bool IsCollision(SphereCollider* collider, CollisionInfo& collisionInfo) override;
    bool IsCollision(BoxCollider* collider, CollisionInfo& collisionInfo) override;

    void SetCenter(const Vector3& center) { sphere_.center = center; }
    void SetRadius(float radius) { sphere_.radius = radius; }

private:
    Math::Sphere sphere_;
};

class BoxCollider :
    public Collider {
    friend class SphereCollider;
public:
    bool IsCollision(Collider* other, CollisionInfo& collisionInfo) override;
    bool IsCollision(SphereCollider* other, CollisionInfo& collisionInfo) override;
    bool IsCollision(BoxCollider* other, CollisionInfo& collisionInfo) override;

    void SetCenter(const Vector3& center) { obb_.center = center; }
    void SetOrientation(const Quaternion& orientation) { obb_.orientation = orientation; }
    void SetSize(const Vector3& size) { obb_.size = size; }

private:
    Math::OBB obb_;
};