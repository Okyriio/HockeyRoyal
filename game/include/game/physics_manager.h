#pragma once
#include "engine/component.h"
#include "engine/entity.h"
#include "maths/angle.h"
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

    struct CircleBody
    {
        core::Vec2f position = core::Vec2f::zero();
        core::Vec2f velocity = core::Vec2f::zero();
        core::degree_t angularVelocity = core::degree_t(0.0f);
        core::degree_t rotation = core::degree_t(0.0f);
        BodyType bodyType = BodyType::DYNAMIC;
        float bounciness = 0.f;
        float radius = 0.30f;
        bool isTrigger = false;
    };

    class OnTriggerInterface
    {
    public:
        virtual ~OnTriggerInterface() = default;
    };


    class CircleManager : public core::ComponentManager<CircleBody, static_cast<core::EntityMask>(core::ComponentType::CIRCLE_BODY2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };

    class PhysicsManager
    {
    public:
        explicit PhysicsManager(core::EntityManager& entityManager);
        bool BodyIntersect(CircleBody body1, CircleBody body2);
        void ResolveBodyIntersect(CircleBody& body1, CircleBody& body2);
        core::Vec2f ContactPoint(const CircleBody& body1, const CircleBody& body2) const;
        core::Vec2f RelocateCenter(const CircleBody& body, const core::Vec2f& v);
        float CalculateDistance(CircleBody body1, CircleBody body2);
        void FixedUpdate(sf::Time dt);

        void AddCircle(core::Entity entity);
        void SetCircle(core::Entity entity, const CircleBody& circle);
        [[nodiscard]] const CircleBody& GetCircle(core::Entity entity) const;

        void RegisterTriggerListener(OnTriggerInterface& collisionInterface);
        void CopyAllComponents(const PhysicsManager& physicsManager);

    private:
        core::EntityManager& entityManager_;
        CircleManager circleManager_;
        core::Action<core::Entity, core::Entity> onTriggerAction_;
    };

}