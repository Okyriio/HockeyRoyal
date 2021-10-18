#include <game/physics_manager.h>

namespace game
{

    PhysicsManager::PhysicsManager(core::EntityManager& entityManager) :
        bodyManager_(entityManager), boxManager_(entityManager), entityManager_(entityManager)
    {

    }

	//Intersect
    bool IntersectRigid(Body& c1, Body& c2)
    {
        return core::CalculateDistance(c1.position, c2.position) < (c1.radius / 100 + c2.radius / 100);
    }

    core::Vec2f IntersectPoint(const Rigidbody& m1, const Rigidbody& m2)
    {

        double ratio = (m1.radius / 100) / ((m1.radius / 100) + (m2.radius / 100));
        return (m2.position - m1.position) * ratio + m1.position;

    }

    core::Vec2f RelocateCenter(const  Rigidbody& rb1, const core::Vec2f& I) {

        //Put the center far enough from the other circles
        double ratio = (rb1.radius / 100) / CalculateDistance(rb1.position, I);
        return (rb1.position - I) * ratio + I;

    }
    void ResolveIntersect(Rigidbody& rb1, Rigidbody& rb2) {

        //Calculates tangeants and normals
        float v1n = ComputeNormal(rb1.position, IntersectPoint(rb1, rb2)).x * rb1.velocity.x +
            ComputeNormal(rb1.position, IntersectPoint(rb1, rb2)).y * rb1.velocity.y;
        float v1t = ComputeTangent(rb1.position, IntersectPoint(rb1, rb2)).x * rb1.velocity.x +
            ComputeTangent(rb1.position, IntersectPoint(rb1, rb2)).y * rb1.velocity.y;
        float v2n = ComputeNormal(rb2.position, IntersectPoint(rb1, rb2)).x * rb2.velocity.x +
            ComputeNormal(rb2.position, IntersectPoint(rb1, rb2)).y * rb2.velocity.y;
        float v2t = ComputeTangent(rb2.position, IntersectPoint(rb1, rb2)).x * rb2.velocity.x +
            ComputeTangent(rb2.position, IntersectPoint(rb1, rb2)).y * rb2.velocity.y;
        //Calculates new velocity and directions after collisions
        rb1.velocity.x = ComputeNormal(rb1.position, IntersectPoint(rb1, rb2)).x * v2n + ComputeTangent(
            rb1.position, IntersectPoint(rb1, rb2)).x * v1t * -rb1.R;
        rb1.velocity.y = ComputeNormal(rb1.position, IntersectPoint(rb1, rb2)).y * v2n + ComputeTangent(
            rb1.position, IntersectPoint(rb1, rb2)).y * v1t * -rb1.R;
        rb2.velocity.x = ComputeNormal(rb2.position, IntersectPoint(rb1, rb2)).x * v1n + ComputeTangent(
            rb2.position, IntersectPoint(rb1, rb2)).x * v2t * -rb2.R;
        rb2.velocity.y = ComputeNormal(rb2.position, IntersectPoint(rb1, rb2)).y * v1n + ComputeTangent(
            rb2.position, IntersectPoint(rb1, rb2)).y * v2t * -rb2.R;

        rb1.position = RelocateCenter(rb1, IntersectPoint(rb1, rb2));
        rb2.position = RelocateCenter(rb2, IntersectPoint(rb1, rb2));
        rb1.velocity = rb1.velocity * -rb1.R;
        rb2.velocity = rb2.velocity * -rb2.R;
    }
    core::Vec2f Body::GetPositionAtTime(float delta_time) const
    {
        return position + velocity * delta_time;
    	
    }

    void PhysicsManager::FixedUpdate(sf::Time dt)
    {
        UpdateDrawData(dt.asSeconds());
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::BODY2D)))
                continue;
            auto body = bodyManager_.GetComponent(entity);
            body.position += body.velocity * dt.asSeconds();
           
            bodyManager_.SetComponent(entity, body);
        }
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::BODY2D) | static_cast<core::EntityMask>(core::ComponentType::BOX_COLLIDER2D))) {
                continue;
            }
            for (core::Entity otherEntity = entity; otherEntity < entityManager_.GetEntitiesSize(); otherEntity++)
            {
                if (entity == otherEntity)
                    continue;
                    if (!entityManager_.HasComponent(otherEntity, static_cast<core::EntityMask>(core::ComponentType::BODY2D) | static_cast<core::EntityMask>(core::ComponentType::BOX_COLLIDER2D))) {
                        
                        continue;
                    }

                const Body& body1 = bodyManager_.GetComponent(entity);
                const Box& box1 = boxManager_.GetComponent(entity);

                const Body& body2 = bodyManager_.GetComponent(otherEntity);
                const Box& box2 = boxManager_.GetComponent(otherEntity);

				 

            }
        
        }
    }

    void PhysicsManager::SetBody(core::Entity entity, const Body& body)
    {
        bodyManager_.SetComponent(entity, body);
    }

    const Body& PhysicsManager::GetBody(core::Entity entity) const
    {
        return bodyManager_.GetComponent(entity);
    }

    void PhysicsManager::AddBody(core::Entity entity)
    {
        bodyManager_.AddComponent(entity);
    }

    void PhysicsManager::AddBox(core::Entity entity)
    {
        boxManager_.AddComponent(entity);
    }

    void PhysicsManager::SetBox(core::Entity entity, const Box& box)
    {
        boxManager_.SetComponent(entity, box);
    }

    const Box& PhysicsManager::GetBox(core::Entity entity) const
    {
        return boxManager_.GetComponent(entity);
    }

    void PhysicsManager::RegisterTriggerListener(OnTriggerInterface& collisionInterface)
    {
       
    }

    void PhysicsManager::CopyAllComponents(const PhysicsManager& physicsManager)
    {
        bodyManager_.CopyAllComponents(physicsManager.bodyManager_.GetAllComponents());
        boxManager_.CopyAllComponents(physicsManager.boxManager_.GetAllComponents());
    }
}
