#pragma once
#include <vector>

#include "../Actor.h"
#include "../components/CollisionComponent.h"
#include "../Debug.h"
#include "../components/PhysicsComponent.h"

struct Sphere;

class CollisionSystem
{
private:
    std::vector<Ref<Actor>> colliding_actors_;

public:
    void AddActor(const Ref<Actor>& actor)
    {
        if (actor->GetComponent<CollisionComponent>().get() == nullptr)
        {
            Debug::Error("The Actor must have a CollisionComponent - ignored ", __FILE__, __LINE__);
            return;
        }

        if (actor->GetComponent<PhysicsComponent>().get() == nullptr)
        {
            Debug::Error("The Actor must have a PhysicsComponent - ignored ", __FILE__, __LINE__);
            return;
        }
        colliding_actors_.push_back(actor);
    }
    
    bool CollisionDetection(const Sphere &s1, const Sphere &s2) const; 
    bool CollisionDetection(const AABB &bb1, const AABB &bb2) const;    
    bool CollisionDetection(const Sphere s1, const MATHEX::Plane p1) const;
    
    void SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2);
    
    void Update(const float deltaTime);
    
};
