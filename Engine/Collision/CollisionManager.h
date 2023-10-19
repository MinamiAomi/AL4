#pragma once
#include <list>

class Collider;

class CollisionManager {
public:
    static CollisionManager* GetInstance();

    void AddCollider(Collider* collider) { colliders_.emplace_back(collider); }
    void RemoveCollider(Collider* collider) { colliders_.remove(collider); }

    void CheckCollision();

private:
    CollisionManager() = default;
    ~CollisionManager() = default;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;
    CollisionManager(CollisionManager&&) = delete;
    CollisionManager& operator=(CollisionManager&&) = delete;

    std::list<Collider*> colliders_;
};