#include <game/physics_manager.h>

namespace game
{

    PhysicsManager::PhysicsManager(core::EntityManager& entityManager) :
        entityManager_(entityManager),
        circleManager_(entityManager)
    {

    }

    
	
    void PhysicsManager::ResolveBodyIntersect(CircleBody& body1, CircleBody& body2)
    {
	    const float v1n = ComputeNormal(body1.position, ContactPoint(body1, body2)).x * body1.velocity.x +
	        ComputeNormal(body1.position, ContactPoint(body1, body2)).y * body1.velocity.y;
	    const float v1t = ComputeTangent(body1.position, ContactPoint(body1, body2)).x * body1.velocity.x +
            ComputeTangent(body1.position, ContactPoint(body1, body2)).y * body1.velocity.y;
	    const float v2n = ComputeNormal(body2.position, ContactPoint(body1, body2)).x * body2.velocity.x +
            ComputeNormal(body2.position, ContactPoint(body1, body2)).y * body2.velocity.y;
	    const float v2t = ComputeTangent(body2.position, ContactPoint(body1, body2)).x * body2.velocity.x +
            ComputeTangent(body2.position, ContactPoint(body1, body2)).y * body2.velocity.y;

        body1.velocity.x = ComputeNormal(body1.position, ContactPoint(body1, body2)).x * v2n + ComputeTangent(
            body1.position, ContactPoint(body1, body2)).x * v1t * -body1.bounciness;
        body1.velocity.y = ComputeNormal(body1.position, ContactPoint(body1, body2)).y * v2n + ComputeTangent(
            body1.position, ContactPoint(body1, body2)).y * v1t * -body1.bounciness;
        body2.velocity.x = ComputeNormal(body2.position, ContactPoint(body1, body2)).x * v1n + ComputeTangent(
            body2.position, ContactPoint(body1, body2)).x * v2t * -body2.bounciness;
        body2.velocity.y = ComputeNormal(body2.position, ContactPoint(body1, body2)).y * v1n + ComputeTangent(
            body2.position, ContactPoint(body1, body2)).y * v2t * -body2.bounciness;

        body1.position = RelocateCenter(body1, ContactPoint(body1, body2));
        body2.position = RelocateCenter(body2, ContactPoint(body1, body2));
        body1.velocity = body1.velocity * -body1.bounciness;
        body2.velocity = body2.velocity * -body2.bounciness;
    }

    core::Vec2f PhysicsManager::ContactPoint(const CircleBody& body1, const CircleBody& body2) const
    {
        double ratio = (body1.radius) / ((body1.radius)+(body2.radius));
        return (body2.position - body1.position) * ratio + body1.position;
    }

    core::Vec2f PhysicsManager::RelocateCenter(const CircleBody& body, const core::Vec2f& v)
    {
        double ratio = (body.radius) / (body.position - v).Length();
        return (body.position - v) * ratio + v;
    }

    float PhysicsManager::CalculateDistance(CircleBody body1, CircleBody body2)
    {
        const float dx = body2.position.x - body1.position.x;
        const float dy = body2.position.y - body1.position.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool PhysicsManager::BodyIntersect(CircleBody body1, CircleBody body2)
    {
        return CalculateDistance(body1, body2) < (body1.radius + body2.radius);
    }
	
    void PhysicsManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::CIRCLE_BODY2D)))
                continue;
            
            core::Vec2f maxPos{(core::windowSize.x / core::pixelPerMeter/2), (core::windowSize.y / core::pixelPerMeter/2) };
            core::Vec2f minPos{ -(core::windowSize.x / core::pixelPerMeter/2), -(core::windowSize.y / core::pixelPerMeter/2) };
            auto body = circleManager_.GetComponent(entity);
            body.position += body.velocity * dt.asSeconds();
            
        	if(body.position.x <= minPos.x + body.radius)
        	{
                body.position.x = minPos.x + body.radius;
                body.velocity.x = -body.velocity.x * body.bounciness;
        	}
        	if(body.position.y <= minPos.y + body.radius)
        	{
                body.position.y = minPos.y + body.radius;
                body.velocity.y = -body.velocity.y * body.bounciness;
        	}
            if (body.position.x >= maxPos.x - body.radius)
            {
                body.position.x = maxPos.x - body.radius;
                body.velocity.x = -body.velocity.x * body.bounciness;
            }
            if (body.position.y >= maxPos.y - body.radius)
            {
                body.position.y = maxPos.y - body.radius;
                body.velocity.y = -body.velocity.y * body.bounciness;
            }
          
            circleManager_.SetComponent(entity, body);
        }
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity,
                                                   static_cast<core::EntityMask>(core::ComponentType::CIRCLE_BODY2D))
                                                    ||
                entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::DESTROYED)))
                continue;
            for (core::Entity otherEntity = entity; otherEntity < entityManager_.GetEntitiesSize(); otherEntity++)
            {
                if (entity == otherEntity)
                    continue;
                if (!entityManager_.HasComponent(otherEntity,
                                                 static_cast<core::EntityMask>(core::ComponentType::CIRCLE_BODY2D)) ||
                    entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::DESTROYED)))
                    continue;
                 CircleBody& body1 = circleManager_.GetComponent(entity);

                 CircleBody& body2 = circleManager_.GetComponent(otherEntity);

                
               if(BodyIntersect(body1, body2))
               {
                   ResolveBodyIntersect(body1, body2);
                   onTriggerAction_.Execute(entity, otherEntity);
               }
                   

            }
        }
    }


    void PhysicsManager::AddCircle(core::Entity entity)
    {
        circleManager_.AddComponent(entity);
    }

    void PhysicsManager::SetCircle(core::Entity entity, const CircleBody& circle)
    {
        circleManager_.SetComponent(entity, circle);
    }

    const CircleBody& PhysicsManager::GetCircle(core::Entity entity) const
    {
        return circleManager_.GetComponent(entity);
    }

    void PhysicsManager::RegisterTriggerListener(OnTriggerInterface& collisionInterface)
    {
       
    }

    void PhysicsManager::CopyAllComponents(const PhysicsManager& physicsManager)
    {
        circleManager_.CopyAllComponents(physicsManager.circleManager_.GetAllComponents());
    }
}