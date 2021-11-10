#pragma once
#include <SFML/System/Time.hpp>

#include "game/game_globals.h"
#include "player_character.h"

namespace game
{
	class PhysicsManager;
    class PlayerCharacterManager;
	
	struct Ball
    {
        PlayerNumber playerNumber = INVALID_PLAYER;

       

    };

    class GameManager;
    class BallManager : public core::ComponentManager<Ball, static_cast<core::EntityMask>(ComponentType::BALL)>
    {
    public:
        explicit BallManager(core::EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager, PlayerCharacterManager& playerManager);
        void FixedUpdate(sf::Time dt);
    private:
        GameManager& gameManager_;
        PhysicsManager& physicsManager_;
        PlayerCharacterManager& playerManager_;
    	
    };
}
