#include "CollisionSystem.h"

#include <cmath>
#include "../structs/Shapes.h"
#include "../TransformComponent.h"

// ---------------------------------------------------------------------------
// Collision Detection
// ---------------------------------------------------------------------------

bool CollisionSystem::CollisionDetection(const Sphere& s1, const Sphere& s2) const
{
    const MATH::Vec3 delta = s1.center - s2.center;
    const float distSq = MATH::VMath::dot(delta, delta);
    const float radiusSum = s1.radius + s2.radius;
    return distSq <= (radiusSum * radiusSum);
}

bool CollisionSystem::CollisionDetection(const AABB& bb1, const AABB& bb2) const
{
    /** Separating-axis test on all three axes **/
    if (std::abs(bb1.center.x - bb2.center.x) > (bb1.halfExtents.x + bb2.halfExtents.x)) return false;
    if (std::abs(bb1.center.y - bb2.center.y) > (bb1.halfExtents.y + bb2.halfExtents.y)) return false;
    if (std::abs(bb1.center.z - bb2.center.z) > (bb1.halfExtents.z + bb2.halfExtents.z)) return false;
    return true;
}

bool CollisionSystem::CollisionDetection(const Sphere s1, const MATHEX::Plane p1) const
{
    /** Plane equation: n·point + negativeDist = 0, so signed dist = n·center + negativeDist **/
    const float dist = MATH::VMath::dot(p1.n, s1.center) + p1.negativeDist;
    return std::abs(dist) <= s1.radius;
}

// ---------------------------------------------------------------------------
// AABB elastic response (file-local helper)
// ---------------------------------------------------------------------------

static void AABBAABBCollisionResponse(
    const AABB& bb1, const Ref<PhysicsComponent>& pc1,
    const AABB& bb2, const Ref<PhysicsComponent>& pc2)
{
    /** Minimum-overlap axis becomes the collision normal **/
    const float overlapX = (bb1.halfExtents.x + bb2.halfExtents.x) - std::abs(bb1.center.x - bb2.center.x);
    const float overlapY = (bb1.halfExtents.y + bb2.halfExtents.y) - std::abs(bb1.center.y - bb2.center.y);
    const float overlapZ = (bb1.halfExtents.z + bb2.halfExtents.z) - std::abs(bb1.center.z - bb2.center.z);

    MATH::Vec3 n(0.0f, 0.0f, 0.0f);
    if (overlapX <= overlapY && overlapX <= overlapZ)
        n.x = (bb1.center.x < bb2.center.x) ? -1.0f : 1.0f;
    else if (overlapY <= overlapX && overlapY <= overlapZ)
        n.y = (bb1.center.y < bb2.center.y) ? -1.0f : 1.0f;
    else
        n.z = (bb1.center.z < bb2.center.z) ? -1.0f : 1.0f;

    constexpr float e = 1.0f;
    const MATH::Vec3 v1 = pc1->velocity_;
    const MATH::Vec3 v2 = pc2->velocity_;
    const float m1 = pc1->mass_;
    const float m2 = pc2->mass_;

    const float v1P = MATH::VMath::dot(v1, n);
    const float v2P = MATH::VMath::dot(v2, n);

    if (v1P - v2P > 0.0f) return;
    if (m1 == 0.0f && m2 == 0.0f) return;

    if (m1 == 0.0f)
    {
        /** bb1 is static — only bb2 bounces **/
        pc2->velocity_ = v2 + (2.0f * e * std::abs(v2P)) * (-n);
        return;
    }
    if (m2 == 0.0f)
    {
        /** bb2 is static — only bb1 bounces **/
        pc1->velocity_ = v1 + (2.0f * e * std::abs(v1P)) * n;
        return;
    }

    const float v1p_new = (((m1 - e * m2) * v1P) + ((1.0f + e) * m2 * v2P)) / (m1 + m2);
    const float v2p_new = (((m2 - e * m1) * v2P) + ((1.0f + e) * m1 * v1P)) / (m1 + m2);
    pc1->velocity_ = v1 + (v1p_new - v1P) * n;
    pc2->velocity_ = v2 + (v2p_new - v2P) * n;
}

// ---------------------------------------------------------------------------
// SphereSphereCollisionResponse  (updated with static-body guard)
// ---------------------------------------------------------------------------

void CollisionSystem::SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2,
                                                    Ref<PhysicsComponent> pc2)
{
    std::cout << "Sphere-Sphere Collision Response" << std::endl;
    const MATH::Vec3 L = s1.center - s2.center;
    const float lenSq = MATH::VMath::dot(L, L);
    if (lenSq < 1e-8f) return; // coincident centers — skip
    const MATH::Vec3 n = L * (1.0f / std::sqrt(lenSq));

    constexpr float e = 1.0f;
    const MATH::Vec3 v1 = pc1->velocity_;
    const MATH::Vec3 v2 = pc2->velocity_;
    const float m1 = pc1->mass_;
    const float m2 = pc2->mass_;

    const float v1P = MATH::VMath::dot(v1, n);
    const float v2P = MATH::VMath::dot(v2, n);

    if (v1P - v2P > 0.0f) return;
    if (m1 == 0.0f && m2 == 0.0f) return;

    if (m1 == 0.0f)
    {
        pc2->velocity_ = v2 + (2.0f * e * std::abs(v2P)) * (-n);
        return;
    }
    if (m2 == 0.0f)
    {
        pc1->velocity_ = v1 + (2.0f * e * std::abs(v1P)) * n;
        return;
    }

    const float v1p_new = (((m1 - e * m2) * v1P) + ((1.0f + e) * m2 * v2P)) / (m1 + m2);
    const float v2p_new = (((m2 - e * m1) * v2P) + ((1.0f + e) * m1 * v1P)) / (m1 + m2);
    pc1->velocity_ = v1 + (v1p_new - v1P) * n;
    pc2->velocity_ = v2 + (v2p_new - v2P) * n;
}

void CollisionSystem::Update(const float /*deltaTime*/)
{
    const size_t count = colliding_actors_.size();
    if (count < 2) return;

    // Reset collision flags from the previous frame
    for (size_t i = 0; i < count; ++i)
    {
        if (const Ref<CollisionComponent> cc = colliding_actors_[i]->GetComponent<CollisionComponent>())
            cc->SetColliding(false);
    }

    for (size_t i = 0; i < count - 1; ++i)
    {
        const Ref<Actor>& actorA = colliding_actors_[i];
        const Ref<CollisionComponent> ccA = actorA->GetComponent<CollisionComponent>();
        const Ref<PhysicsComponent> pcA = actorA->GetComponent<PhysicsComponent>();
        if (!ccA || !pcA) continue;

        /** Full world matrix — identical to what the wireframe renderer uses **/
        const MATH::Matrix4 worldMatA = actorA->GetModelMatrix();

        for (size_t j = i + 1; j < count; ++j)
        {
            const Ref<Actor>& actorB = colliding_actors_[j];
            const Ref<CollisionComponent> ccB = actorB->GetComponent<CollisionComponent>();
            const Ref<PhysicsComponent> pcB = actorB->GetComponent<PhysicsComponent>();
            if (!ccB || !pcB) continue;

            const MATH::Matrix4 worldMatB = actorB->GetModelMatrix();
            const Collider_type typeA = ccA->GetType();
            const Collider_type typeB = ccB->GetType();

            if (typeA == Collider_type::SPHERE && typeB == Collider_type::SPHERE)
            {
                const float sA = ccA->GetActorScale();
                const float sB = ccB->GetActorScale();

                /** Center: the wireframe geometry stores (localOffset / actorScale) in mesh space.
                 *  Transforming that point by the world matrix yields the world-space center. **/
                const auto centerA = MATH::Vec3(worldMatA * MATH::Vec4(ccA->GetLocalOffset() * (1.0f / sA), 1.0f));
                const auto centerB = MATH::Vec3(worldMatB * MATH::Vec4(ccB->GetLocalOffset() * (1.0f / sB), 1.0f));

                /** Radius: (radius / actorScale) as a direction vector — world matrix applies all
                 *  ancestor scales, then magnitude gives the true world-space radius. **/
                const float rA = MATH::VMath::mag(MATH::Vec3(worldMatA * MATH::Vec4(ccA->GetRadius() / sA, 0.0f, 0.0f, 0.0f)));
                const float rB = MATH::VMath::mag(MATH::Vec3(worldMatB * MATH::Vec4(ccB->GetRadius() / sB, 0.0f, 0.0f, 0.0f)));

                const Sphere sphA{rA, centerA};
                const Sphere sphB{rB, centerB};

                if (CollisionDetection(sphA, sphB))
                {
                    ccA->SetColliding(true);
                    ccB->SetColliding(true);
                    SphereSphereCollisionResponse(sphA, pcA, sphB, pcB);
                }
            }
            else if (typeA == Collider_type::AABB && typeB == Collider_type::AABB)
            {
                const float sA = ccA->GetActorScale();
                const float sB = ccB->GetActorScale();

                /** World-space center: (localOffset + aabb.center) / actorScale is the mesh-space
                 *  center used by the wireframe — transform it by the world matrix. **/
                const auto wCenterA = MATH::Vec3(worldMatA * MATH::Vec4(
                    (ccA->GetLocalOffset() + ccA->GetAABB().center) * (1.0f / sA), 1.0f));
                const auto wCenterB = MATH::Vec3(worldMatB * MATH::Vec4(
                    (ccB->GetLocalOffset() + ccB->GetAABB().center) * (1.0f / sB), 1.0f));

                /** World-space half extents: project each local half-axis through the world matrix
                 *  (OBB -> world AABB) so the result matches the rendered wireframe exactly. **/
                const MATH::Vec3 hA = ccA->GetAABB().halfExtents * (1.0f / sA);
                const auto hAxW = MATH::Vec3(worldMatA * MATH::Vec4(hA.x, 0.0f, 0.0f, 0.0f));
                const auto hAyW = MATH::Vec3(worldMatA * MATH::Vec4(0.0f, hA.y, 0.0f, 0.0f));
                const auto hAzW = MATH::Vec3(worldMatA * MATH::Vec4(0.0f, 0.0f, hA.z, 0.0f));
                const MATH::Vec3 wHalfA(
                    std::abs(hAxW.x) + std::abs(hAyW.x) + std::abs(hAzW.x),
                    std::abs(hAxW.y) + std::abs(hAyW.y) + std::abs(hAzW.y),
                    std::abs(hAxW.z) + std::abs(hAyW.z) + std::abs(hAzW.z)
                );

                const MATH::Vec3 hB = ccB->GetAABB().halfExtents * (1.0f / sB);
                const auto hBxW = MATH::Vec3(worldMatB * MATH::Vec4(hB.x, 0.0f, 0.0f, 0.0f));
                const auto hByW = MATH::Vec3(worldMatB * MATH::Vec4(0.0f, hB.y, 0.0f, 0.0f));
                const auto hBzW = MATH::Vec3(worldMatB * MATH::Vec4(0.0f, 0.0f, hB.z, 0.0f));
                const MATH::Vec3 wHalfB(
                    std::abs(hBxW.x) + std::abs(hByW.x) + std::abs(hBzW.x),
                    std::abs(hBxW.y) + std::abs(hByW.y) + std::abs(hBzW.y),
                    std::abs(hBxW.z) + std::abs(hByW.z) + std::abs(hBzW.z)
                );

                const AABB wA{wCenterA, wHalfA};
                const AABB wB{wCenterB, wHalfB};

                if (CollisionDetection(wA, wB))
                {
                    ccA->SetColliding(true);
                    ccB->SetColliding(true);
                    AABBAABBCollisionResponse(wA, pcA, wB, pcB);
                }
            }
        }
    }
}
