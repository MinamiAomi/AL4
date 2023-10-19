#include "Collider.h"

#include "CollisionManager.h"

Collider::Collider() {
    CollisionManager::GetInstance()->AddCollider(this);
}

Collider::~Collider() {
    CollisionManager::GetInstance()->RemoveCollider(this);
}

bool Collider::CanCollision(Collider* other) const {
    return (this->collisionAttribute_ & other->collisionMask_) && (other->collisionAttribute_ & this->collisionMask_);
}

bool SphereCollider::IsCollision(Collider* other, CollisionInfo& collisionInfo) {
    if (CanCollision(other)) {
        return  other->IsCollision(this, collisionInfo);
    }
    return false;
}

bool SphereCollider::IsCollision(SphereCollider* other, CollisionInfo& collisionInfo) {
    // 差分ベクトルを求める
    Vector3 diff = other->sphere_.center - this->sphere_.center;
    float hitRange = this->sphere_.radius + other->sphere_.radius;
    if (diff.LengthSquare() > hitRange * hitRange) {
        return false;
    }

    // 衝突情報を格納していく
    float length = diff.Length();
    collisionInfo.collider = this;
    if (length != 0.0f) {
        collisionInfo.normal = diff / length;
    }
    else {
        collisionInfo.normal = Vector3::zero;
    }
    collisionInfo.depth = hitRange - length;
    return true;
}

bool SphereCollider::IsCollision(BoxCollider* other, CollisionInfo& collisionInfo) {
    Math::Sphere& sphere = this->sphere_;
    Math::OBB& obb = other->obb_;
    // obbのローカル空間で衝突判定を行う
    Vector3 centerInOBBLocal = obb.orientation.Inverse() * (sphere.center - obb.center);
    Math::AABB aabbInOBBLocal{ -obb.size, obb.size };
    Math::Sphere sphereInOBBLocal{ centerInOBBLocal, sphere.radius };

    Vector3 point = {
          std::clamp(centerInOBBLocal.x, -obb.size.x, obb.size.x),
          std::clamp(centerInOBBLocal.y, -obb.size.y, obb.size.y),
          std::clamp(centerInOBBLocal.z, -obb.size.z, obb.size.z) };
    Vector3 diff = point - centerInOBBLocal;

    if (diff.LengthSquare() > sphere.radius * sphere.radius) {
        return false;
    }

    // 衝突情報を格納していく
    float length = diff.Length();
    collisionInfo.collider = this;
    if (length != 0.0f) {
        collisionInfo.normal = obb.orientation * (diff / length);
    }
    else {
        collisionInfo.normal = Vector3::zero;
    }
    collisionInfo.depth = sphere.radius - length;
    return true;
}

bool BoxCollider::IsCollision(Collider* other, CollisionInfo& collisionInfo) {
    if (CanCollision(other)) {
        return  other->IsCollision(this, collisionInfo);
    }
    return false;
}

bool BoxCollider::IsCollision(SphereCollider* other, CollisionInfo& collisionInfo) {
    Math::Sphere& sphere = other->sphere_;
    Math::OBB& obb = this->obb_;
    // obbのローカル空間で衝突判定を行う
    Vector3 centerInOBBLocal = obb.orientation.Inverse() * (sphere.center - obb.center);
    Math::AABB aabbInOBBLocal{ -obb.size, obb.size };
    Math::Sphere sphereInOBBLocal{ centerInOBBLocal, sphere.radius };

    Vector3 point = {
          std::clamp(centerInOBBLocal.x, -obb.size.x, obb.size.x),
          std::clamp(centerInOBBLocal.y, -obb.size.y, obb.size.y),
          std::clamp(centerInOBBLocal.z, -obb.size.z, obb.size.z) };
    Vector3 diff = centerInOBBLocal - point;

    if (diff.LengthSquare() > sphere.radius * sphere.radius) {
        return false;
    }

    // 衝突情報を格納していく
    float length = diff.Length();
    collisionInfo.collider = this;
    if (length != 0.0f) {
        collisionInfo.normal = obb.orientation * (diff / length);
    }
    else {
        collisionInfo.normal = Vector3::zero;
    }
    collisionInfo.depth = sphere.radius - length;
    return true;
}

bool BoxCollider::IsCollision(BoxCollider* other, CollisionInfo& collisionInfo) {
    Vector3 vertices1[] = {
         { -this->obb_.size.x, -this->obb_.size.y, -this->obb_.size.z },   // 左下前
         { -this->obb_.size.x,  this->obb_.size.y, -this->obb_.size.z },   // 左上前
         {  this->obb_.size.x,  this->obb_.size.y, -this->obb_.size.z },   // 右上前
         {  this->obb_.size.x, -this->obb_.size.y, -this->obb_.size.z },   // 右下前
         { -this->obb_.size.x, -this->obb_.size.y,  this->obb_.size.z },   // 左下奥 
         { -this->obb_.size.x,  this->obb_.size.y,  this->obb_.size.z },   // 左上奥
         {  this->obb_.size.x,  this->obb_.size.y,  this->obb_.size.z },   // 右上奥
         {  this->obb_.size.x, -this->obb_.size.y,  this->obb_.size.z },   // 右下奥
    };

    Vector3 vertices2[] = {
         { -other->obb_.size.x, -other->obb_.size.y, -other->obb_.size.z },   // 左下前
         { -other->obb_.size.x,  other->obb_.size.y, -other->obb_.size.z },   // 左上前
         {  other->obb_.size.x,  other->obb_.size.y, -other->obb_.size.z },   // 右上前
         {  other->obb_.size.x, -other->obb_.size.y, -other->obb_.size.z },   // 右下前
         { -other->obb_.size.x, -other->obb_.size.y,  other->obb_.size.z },   // 左下奥 
         { -other->obb_.size.x,  other->obb_.size.y,  other->obb_.size.z },   // 左上奥
         {  other->obb_.size.x,  other->obb_.size.y,  other->obb_.size.z },   // 右上奥
         {  other->obb_.size.x, -other->obb_.size.y,  other->obb_.size.z },   // 右下奥
    };

    const size_t numVertices = _countof(vertices1);
    for (size_t i = 0; i < numVertices; ++i) {
        vertices1[i] = this->obb_.orientation * vertices1[i] + this->obb_.center;
        vertices2[i] = other->obb_.orientation * vertices2[i] + other->obb_.center;
    }

    Vector3 axes1[] = {
        this->obb_.orientation.GetRight(),
        this->obb_.orientation.GetUp(),
        this->obb_.orientation.GetForward(),
    };

    Vector3 axes2[] = {
        other->obb_.orientation.GetRight(),
        other->obb_.orientation.GetUp(),
        other->obb_.orientation.GetForward(),
    };
    const size_t numAxes = _countof(axes1);

    float minOverlap = FLT_MAX;
    Vector3 minOverlapAxis = {};

    // 分離軸判定関数
    auto IsSeparateAxis = [&](const Vector3& axis) {
        float min1 = 0, max1 = 0;
        float min2 = 0, max2 = 0;
        min1 = max1 = Dot(axis, vertices1[0]);
        min2 = max2 = Dot(axis, vertices2[0]);
        for (size_t i = 1; i < numVertices; ++i) {
            float dot1 = Dot(axis, vertices1[i]);
            min1 = std::min(dot1, min1);
            max1 = std::max(dot1, max1);
        }
        for (size_t i = 1; i < numVertices; ++i) {
            float dot2 = Dot(axis, vertices2[i]);
            min2 = std::min(dot2, min2);
            max2 = std::max(dot2, max2);
        }
        float overlap = max1 - min1 + max2 - min2;
        float maxOverlap = std::max(max1, max2) - std::min(min1, min2);

        // 最も幅の狭い分離軸
        if (overlap < minOverlap) {
            minOverlapAxis = axis;
            minOverlap = overlap;
        }

        return overlap < maxOverlap;
    };

    for (size_t i = 0; i < numAxes; ++i) {
        if (IsSeparateAxis(axes1[i])) { return false; }
    }
    for (size_t i = 0; i < numAxes; ++i) {
        if (IsSeparateAxis(axes2[i])) { return false; }
    }
    for (size_t i = 0; i < numAxes; ++i) {
        for (size_t j = 0; j < numAxes; ++j) {
            if (IsSeparateAxis(Cross(axes1[i], axes2[j]))) { return false; }
        }
    }

    // 衝突情報を格納していく
    collisionInfo.collider = this;
    collisionInfo.normal = minOverlapAxis.Normalized();
    collisionInfo.depth = minOverlap;
    return true;
}
