#include "CollisionSystem.h"

#include "../structs/Shapes.h"

void CollisionSystem::SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2,
                                                    Ref<PhysicsComponent> pc2)
{
    const float e = 1.0f; // coefficient of restitution
    const MATH::Vec3 L = s1.center - s2.center;
    const MATH::Vec3 n = MATH::VMath::normalize(L);
    const MATH::Vec3 v1 = pc1->velocity_;
    const MATH::Vec3 v2 = pc2->velocity_;
    const float m1 = pc1->mass_;
    const float m2 = pc2->mass_;

    const float v1P  = MATH::VMath::dot(v1, n);
    const float v2P = MATH::VMath::dot(v2, n);

    if (v1P - v2P > 0.0f)
    {
        /// The colliding objects are not yet free from one and another, come back next cycle
        return;
    }
    const float v1p_new = (((m1 - e * m2) * v1P) + ((1.0f + e) * m2 * v2P)) / (m1 + m2);
    const float v2p_new = (((m2 - e * m1) * v2P) + ((1.0f + e) * m1 * v1P)) / (m1 + m2);

    pc1->velocity_ = v1 + (v1p_new - v1P) * n;
    pc2->velocity_ = v2 + (v2p_new - v2P) * n;
}
