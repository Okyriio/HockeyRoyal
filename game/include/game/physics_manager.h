#pragma once
#include "game_globals.h"
#include "engine/component.h"
#include "engine/entity.h"

#include "maths/vec2.h"

#include <SFML/System/Time.hpp>

#include "utils/action_utility.h"

namespace game
{
    enum class BodyType
    {
        DYNAMIC,
        STATIC
    };
    struct Body
    {
        core::Vec2f position ;
        core::Vec2f velocity ;
        static constexpr float radius = 78.0f;
        core::Vec2f normal;
        core::Vec2f tang;
        float mass;
        static constexpr core::Vec2f G = { 0.0f, 0.0f }; // G (9.81 m/s^2), can set up teh gravity on here
        static constexpr float BallBounciness = 0.80F;
        core::Vec2f GetPositionAtTime(float delta_time) const;
        BodyType bodyType = BodyType::DYNAMIC;
    };

	struct Ball : Body
	{
        static constexpr float radius = 40.0f;
	};
	
    struct Box : Body
    {
        static constexpr core::Vec2f G = { 0.0f, 0.0f }; // G (9.81 m/s^2), can set up the gravity on here

        void UpdateVariable(double delta_time, int dx, int dy);

        core::Vec2f GetPrintingCenter() const { return position; }
        /*Box& operator=(Box& CM)

        {
            velocity = CM.velocity;
            position = CM.position;
            radius = CM.radius;
            return *this;
        }*/
    protected:
        // Just bouncing the borders.
        void CorrectVelocity(int dx, int dy);

        void CorrectPosition(int dx, int dy);

    private:
		int dx_ = 1000;
        int dy_ = 700;
        double printing_m_ = 100.0;
      
    };



   
    core::Vec2f RelocateCenter(const Body& rb1, const core::Vec2f& I);

   
    class OnTriggerInterface
    {
    public:
        virtual ~OnTriggerInterface() = default;
       
    };

    class BodyManager : public core::ComponentManager<Body, static_cast<core::EntityMask>(core::ComponentType::BODY2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };
	
    class BallManager : public core::ComponentManager<Ball, static_cast<core::EntityMask>(core::ComponentType::BODY2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };
	
    class BoxManager : public core::ComponentManager<Box, static_cast<core::EntityMask>(core::ComponentType::BOX_COLLIDER2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };

    class PhysicsManager
    {
    public:
        explicit PhysicsManager(core::EntityManager& entityManager);
        void FixedUpdate(sf::Time dt);
        [[nodiscard]] const Body& GetBody(core::Entity entity) const;
        void SetBody(core::Entity entity, const Body& body);
        void AddBody(core::Entity entity);

        void AddBox(core::Entity entity);
        void SetBox(core::Entity entity, const Box& box);
        [[nodiscard]] const Box& GetBox(core::Entity entity) const;

        void AddCircle(core::Entity entity);
        void SetCircle(core::Entity entity, const Ball& ball);
        [[nodiscard]] const Ball& GetCircle(core::Entity entity) const;
    	
        void RegisterTriggerListener(OnTriggerInterface& collisionInterface);
        void CopyAllComponents(const PhysicsManager& physicsManager);
    	
       /* void ResolveCollisions(float delta_time);
       
    	
        void Tick(double delta_time);*/
        
    private:
        bool Intersect(const Body c1, const Body& c2);
        void ResolveIntersect(Body& rb1, Body& rb2);
        core::EntityManager& entityManager_;
        BodyManager bodyManager_;
        BallManager ballManager_;
        BoxManager boxManager_;
        core::Action<core::Entity, core::Entity> onTriggerAction_;
    
    };

}
