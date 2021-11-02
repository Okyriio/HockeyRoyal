#include <game/physics_manager.h>

namespace game
{

    PhysicsManager::PhysicsManager(core::EntityManager& entityManager) :
        bodyManager_(entityManager), boxManager_(entityManager), entityManager_(entityManager), ballManager_(entityManager)
    {

    }

    void Box::UpdateVariable(double delta_time, int dx, int dy)
    {
	    core::Vec2f previous_center = position;
	    position =
		    0.5 * G * delta_time * delta_time + // 1/2*G*t^2
		    velocity * delta_time // + v0*t
		    + position; // + x0
	    // delta x / delta t.
	    velocity = (position - previous_center) * 1.0f / delta_time;
	    position = position * printing_m_;
	    CorrectPosition(dx, dy);
	    CorrectVelocity(dx, dy);
	    position = position * 1 / printing_m_;
    }

    void Box::CorrectVelocity(int dx, int dy)
    {
	    if (position.x <= 0 + radius)
		    velocity.x = -BallBounciness * velocity.x;
	    if (position.y <= 0 + radius)
		    velocity.y = -BallBounciness * velocity.y;
	    if (position.x >= (dx - radius))
		    velocity.x = -BallBounciness * velocity.x;
	    if (position.y >= (dy - radius))
		    velocity.y = -BallBounciness * velocity.y;
    }

    void Box::CorrectPosition(int dx, int dy)
    {
	    core::Vec2f previous_center = position;
	    if (position.x <= 0 + radius)
		    position.x = radius;
	    if (position.y <= 0 + radius)
		    position.y = radius;
	    if (position.x >= (dx - radius))
		    position.x = dx - radius;
	    if (position.y >= (dy - radius))
		    position.y = dy - radius;
    }

    bool PhysicsManager::Intersect(const Body c1,const Body& c2)
    {
        return core::CalculateDistance(c1.position, c2.position) < (c1.radius / 100 + c2.radius / 100);
    }

    core::Vec2f IntersectPoint(const Body& m1, const Body& m2)
    {

        double ratio = (m1.radius / 100) / ((m1.radius / 100) + (m2.radius / 100));
        return (m2.position - m1.position) * ratio + m1.position;

    }

  

    core::Vec2f RelocateCenter(const Body& rb1, const core::Vec2f& I) {

        //Put the center far enough from the other circles
        double ratio = (rb1.radius / 100) / CalculateDistance(rb1.position, I);
        return (rb1.position - I) * ratio + I;

    }
    void PhysicsManager::ResolveIntersect(Body& rb1, Body& rb2) {

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
            rb1.position, IntersectPoint(rb1, rb2)).x * v1t * -rb1.BallBounciness;
        rb1.velocity.y = ComputeNormal(rb1.position, IntersectPoint(rb1, rb2)).y * v2n + ComputeTangent(
            rb1.position, IntersectPoint(rb1, rb2)).y * v1t * -rb1.BallBounciness;
        rb2.velocity.x = ComputeNormal(rb2.position, IntersectPoint(rb1, rb2)).x * v1n + ComputeTangent(
            rb2.position, IntersectPoint(rb1, rb2)).x * v2t * -rb2.BallBounciness;
        rb2.velocity.y = ComputeNormal(rb2.position, IntersectPoint(rb1, rb2)).y * v1n + ComputeTangent(
            rb2.position, IntersectPoint(rb1, rb2)).y * v2t * -rb2.BallBounciness;

        rb1.position = RelocateCenter(rb1, IntersectPoint(rb1, rb2));
        rb2.position = RelocateCenter(rb2, IntersectPoint(rb1, rb2));
        rb1.velocity = rb1.velocity * -rb1.BallBounciness;
        rb2.velocity = rb2.velocity * -rb2.BallBounciness;
    }
    core::Vec2f Body::GetPositionAtTime(float delta_time) const
    {
        return position + velocity * delta_time;
    	
    }
	

    void PhysicsManager::FixedUpdate(sf::Time dt)
    {
        
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::BODY2D)))
                continue;
            auto body = bodyManager_.GetComponent(entity);
            core::Vec2f max_pos = { (core::windowSize.y / core::pixelPerMeter / 2),
            	(core::windowSize.x / core::pixelPerMeter / 2) };
            core::Vec2f min_pos = { -(core::windowSize.y / core::pixelPerMeter / 2),
            	-(core::windowSize.x / core::pixelPerMeter / 2) };
            body.position += body.velocity * dt.asSeconds();

            
            /*Ball ball = ballManager_.GetComponent(2);
            ball.position += body.velocity * dt.asSeconds();
            ballManager_.SetComponent(entity, ball);*/
           

           

            if (body.position.x <= min_pos.x + Body::radius/core::pixelPerMeter)
            {
                body.position.x = min_pos.x + Body::radius / core::pixelPerMeter;
                body.velocity.x = 15;
            }
            if (body.position.y <= min_pos.y + Body::radius / core::pixelPerMeter)            //correct position and velocity
            {
                body.position.y = min_pos.y + Body::radius / core::pixelPerMeter;
                body.velocity.y = -body.velocity.y/2;
            }
            if (body.position.x >= max_pos.x - Body::radius / core::pixelPerMeter)
            {
                body.position.x = max_pos.x - Body::radius / core::pixelPerMeter;
                body.velocity.x = -15;
            }
            if (body.position.y >= max_pos.y - Body::radius / core::pixelPerMeter)
            {
                body.position.y = max_pos.y - Body::radius / core::pixelPerMeter;
                body.velocity.y = -10;
            }
            bodyManager_.SetComponent(entity, body);
            auto body1 = bodyManager_.GetComponent(0);
            auto body2 = bodyManager_.GetComponent(1);
        	
            if (Intersect(body1, body2))//Resolve intersections between two pads
            {
                ResolveIntersect(body1, body2);
                bodyManager_.SetComponent(0, body1);
                bodyManager_.SetComponent(1, body2);
            }
          
        }
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::BODY2D) | static_cast<core::EntityMask>(core::ComponentType::BOX_COLLIDER2D))) {
                continue;
            }
            for (core::Entity otherEntity = entity; otherEntity < entityManager_.GetEntitiesSize(); otherEntity++)
            {
                if (entity == otherEntity)
                {
                    continue;
                }
                   
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
    void PhysicsManager::AddCircle(core::Entity entity)
    {
        ballManager_.AddComponent(entity);
    }

    void PhysicsManager::SetCircle(core::Entity entity, const Ball& ball)
    {
        ballManager_.SetComponent(entity, ball);
    }

    const Ball& PhysicsManager::GetCircle(core::Entity entity) const
    {
        return  ballManager_.GetComponent(entity);
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
