#pragma once
#include <SFML/System/Time.hpp>

#include "game/game_globals.h"

namespace game
{
	class PhysicsManager;

	struct Ball
    {
        PlayerNumber playerNumber = INVALID_PLAYER;
    };

    class GameManager;
    class BallManager : public core::ComponentManager<Ball, static_cast<core::EntityMask>(ComponentType::BALL)>
    {
    public:
        explicit BallManager(core::EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager);
        void FixedUpdate(sf::Time dt);
    private:
        GameManager& gameManager_;
        PhysicsManager& physicsManager_;
    };
}
