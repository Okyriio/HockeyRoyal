#pragma once
#include <SFML/System/Time.hpp>

#include "game_globals.h"

namespace game
{
	class BallManager;
	class PhysicsManager;

    struct PlayerCharacter
    {
       
        PlayerInput input = 0u;
        PlayerNumber playerNumber = INVALID_PLAYER;
        int WinPoints = 0;

       
    };
    class GameManager;
    class PlayerCharacterManager : public core::ComponentManager<PlayerCharacter, core::EntityMask(ComponentType::PLAYER_CHARACTER)>
    {
    public:
         PlayerCharacterManager(core::EntityManager& entityManager, PhysicsManager& physicsManager, GameManager& gameManager);
        void FixedUpdate(sf::Time dt);

    private:
        PhysicsManager& physicsManager_;
        GameManager& gameManager_;
        
    };
}
