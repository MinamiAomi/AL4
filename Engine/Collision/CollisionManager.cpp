#include "CollisionManager.h"

#include "Collider.h"

CollisionManager* CollisionManager::GetInstance() {
    static CollisionManager instance;
    return &instance;
}

void CollisionManager::CheckCollision() {
    for (auto iter1 = colliders_.begin(); iter1 != colliders_.end(); ++iter1) {
        Collider* collider1 = *iter1;
        // アクティブじゃなければ通さない
        if (!collider1->isActive_) { continue; }

        for (auto iter2 = iter1; iter2 != colliders_.end(); ++iter2) {
            Collider* collider2 = *iter2;
            // アクティブじゃなければ通さない
            if (!collider2->isActive_) { continue; }

            CollisionInfo collisionInfo;
            if (collider1->IsCollision(collider1, collisionInfo)) {
               // if(!collider1->)
            }
        }
    }

}
