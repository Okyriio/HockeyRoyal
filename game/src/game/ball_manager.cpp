
#include "game/ball_manager.h"


#include "game/game_manager.h"
#include "game/physics_manager.h"
#include "maths/basic.h"


namespace game
{
    BallManager::BallManager(core::EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager, PlayerCharacterManager& playerManager) :
        ComponentManager(entityManager), gameManager_(gameManager), physicsManager_(physicsManager), playerManager_(playerManager)
    {

    }


    void BallManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            const core::Vec2f maxPos{ (core::windowSize.x / core::pixelPerMeter / 2), (core::windowSize.y / core::pixelPerMeter / 2) };
            const core::Vec2f minPos{ -(core::windowSize.x / core::pixelPerMeter / 2), -(core::windowSize.y / core::pixelPerMeter / 2) };

            
            auto& ballBody = physicsManager_.GetCircle(entity);
            const auto player1 = gameManager_.GetEntityFromPlayerNumber(0);
            const auto player2 = gameManager_.GetEntityFromPlayerNumber(1);
            auto& playerCharacter1 = playerManager_.GetComponent(player1);
            auto& playerCharacter2 = playerManager_.GetComponent(player2);
            int goalSizeLeft = -2;
            int goalSizeRight = 2;
            
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::BALL)))
            {
                playerManager_.SetComponent(entity, playerCharacter1);
                playerManager_.SetComponent(entity, playerCharacter2);
            }

            //This makes it so only the ball counts in the goals
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER)))
            {
                //Checks if the ball is in the goals
                if (ballBody.position.y == minPos.y + ballBody.radius && 
                    ballBody.position.x >= goalSizeLeft && ballBody.position.x <= goalSizeRight)
                {

                    playerCharacter1.WinPoints += 1;
                }

                if (ballBody.position.y == maxPos.y - ballBody.radius &&
                    ballBody.position.x >= goalSizeLeft && ballBody.position.x <= goalSizeRight)
                {

                    playerCharacter2.WinPoints += 1;
                }
            }
           

        }
    }
}
