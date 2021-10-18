#include <game/player_character.h>
#include <game/game_manager.h>

namespace game
{
    PlayerCharacterManager::PlayerCharacterManager(core::EntityManager& entityManager, PhysicsManager& physicsManager, GameManager& gameManager) :
        ComponentManager(entityManager),
        physicsManager_(physicsManager),
        gameManager_(gameManager)

    {

    }

    void PlayerCharacterManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity playerEntity = 0; playerEntity < entityManager_.GetEntitiesSize(); playerEntity++)
        {
            if (!entityManager_.HasComponent(playerEntity,
                                                   static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER)))
                continue;
            auto playerBody = physicsManager_.GetBody(playerEntity);
            auto playerCharacter = GetComponent(playerEntity);
            const auto input = playerCharacter.input;

            const bool right = input & PlayerInputEnum::PlayerInput::RIGHT;
            const bool left = input & PlayerInputEnum::PlayerInput::LEFT;
            const bool up = input & PlayerInputEnum::PlayerInput::UP;
            const bool down = input & PlayerInputEnum::PlayerInput::DOWN;

            sf::Vector2<float> dirV = {0.0f, playerSpeed};
            sf::Vector2<float> dirH = { playerSpeed, 0.0f };

            const auto accelerationV = ((up ? 0.0f : -10.0f) + (down ? 0.0f : 10.0f)) * dirV;
            const auto accelerationH = ((left ? -10.0f : 0.0f) + (right ? 10.0f : 0.0f)) * dirH;

            playerBody.velocity = (accelerationH + accelerationV) * dt.asSeconds();
          

            physicsManager_.SetBody(playerEntity, playerBody);
    
        }
    }
}
