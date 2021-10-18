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
        core::Vec2f position = core::Vec2f::zero();
        core::Vec2f velocity = core::Vec2f::zero();
        float radius;
        core::Vec2f normal;
        core::Vec2f tang;
        float mass;
        core::Vec2f GetPositionAtTime(float delta_time) const;
        BodyType bodyType = BodyType::DYNAMIC;
    };
   

    bool Intersect(Body c1, Body c2);
	
    struct Rigidbody : public Body
    {
        const float R = 0.80F;
        bool operator==(Rigidbody rb);
    };
    bool IntersectRigid(Body& c1, Body& c2);
    void ResolveIntersect(Rigidbody& rb1, Rigidbody& rb2);
    core::Vec2f RelocateCenter(const Rigidbody& rb1, const core::Vec2f& I);

    struct CircleMRUA : public Rigidbody {


        const core::Vec2f G = { 0.0f, 0.0f }; // G (9.81 m/s^2), can set up teh gravity on here

        void UpdateVariable(double delta_time, int dx, int dy) {
	        core::Vec2f previous_center = position;
            position =
                0.5 * G * delta_time * delta_time + // 1/2*G*t^2
                velocity * delta_time				// + v0*t
                + position;							// + x0
            // delta x / delta t.
            velocity = (position  - previous_center) * 1.0f / delta_time;
            printing_center_ = position * printing_m_;
            CorrectPosition(dx, dy);
            CorrectVelocity(dx, dy);
            position = printing_center_ * 1 / printing_m_;
        }

        core::Vec2f GetPrintingCenter() const { return printing_center_; }
        CircleMRUA& operator=(CircleMRUA& CM)

        {
            velocity = CM.velocity;
            position = CM.position;
            radius = CM.radius;
            return *this;
        }
    protected:
        // Just bouncing the borders.
        void CorrectVelocity(int dx, int dy) {
            if (printing_center_.x <= 0 + radius)
                velocity.x = -R * velocity.x;
            if (printing_center_.y <= 0 + radius)
                velocity.y = -R * velocity.y;
            if (printing_center_.x >= (dx - radius))
                velocity.x = -R * velocity.x;
            if (printing_center_.y >= (dy - radius))
                velocity.y = -R * velocity.y;
        }
        void CorrectPosition(int dx, int dy) {
	        core::Vec2f previous_center = printing_center_;
            if (printing_center_.x <= 0 + radius)
                printing_center_.x = radius;
            if (printing_center_.y <= 0 + radius)
                printing_center_.y = radius;
            if (printing_center_.x >= (dx - radius))
                printing_center_.x = dx - radius;
            if (printing_center_.y >= (dy - radius))
                printing_center_.y = dy - radius;
        }

    private:
        double printing_m_ = 100.0;
        core::Vec2f printing_center_ = position * printing_m_;
    };

   
    struct Box
    {
        core::Vec2f extends = core::Vec2f::one();
        bool isTrigger = false;
    };

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

        void RegisterTriggerListener(OnTriggerInterface& collisionInterface);
        void CopyAllComponents(const PhysicsManager& physicsManager);
        void Simulate(float delta_time)
        {
            //for every circles do the intersect, and then the resolve
            std::vector<CircleMRUA> vrb = circles_;
            for (auto first = vrb.begin(); first != vrb.end(); ++first) {
                for (auto second = vrb.begin(); second != vrb.end(); ++second) {
                    if (first >= second) continue;
                    if (IntersectRigid(*first, *second))
                    {
                        ResolveIntersect(*first, *second);
                    }
                }
            }
            circles_ = vrb;
        }
        void UpdateDrawData(double delta_time) {
            float new_time = delta_time * 1.0;
            for (auto& circle : circles_) {
                circle.UpdateVariable(new_time, dx_, dy_);

            }
            Simulate(new_time);
        }
    private:
        core::EntityManager& entityManager_;
        BodyManager bodyManager_;
        BoxManager boxManager_;
        int dx_ = 1000;
        int dy_ = 700;
        core::Action<core::Entity, core::Entity> onTriggerAction_;
        std::vector<CircleMRUA> circles_;
        
    };

}
