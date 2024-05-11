#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../ecs/component.hpp"
// #include "../ecs/laser-bullet.hpp"

#include <iostream>

namespace our
{
    enum class EnemyType
    {
        SHOOTER,
        MELEE,
        BOSS1,
        BOSS2,
        HEAL
    };

    // Abstract class for all enemies
    class EnemyComponent : public Component
    {
        float range;
        float health;
        int weapon;
        float rateOfFire = 25;
        int deltasCounter = 0;
        bool zigZag = true;
        float timeSinceLastInversion = 0.0f;
        float inversionInterval = 0.1f;

    public:
        our::EnemyType type;

        static std::string getID() { return "Enemy"; }

        EnemyComponent() = default;

        void deserialize(const nlohmann::json &data) override
        {
            if (!data.is_object())
                return;

            // get the enemy type
            std::string enemyTypeStr = data.value("enemyType", "shooter");
            if (enemyTypeStr == "shooter")
            {
                type = EnemyType::SHOOTER;
                range = 20;
            }
            else if (enemyTypeStr == "melee")
            {
                type = EnemyType::MELEE;
                range = 40;
            }
            else if (enemyTypeStr == "boss1")
            {
                type = EnemyType::BOSS1;
                rateOfFire=5;
                range = 30;
            }
            else if (enemyTypeStr == "boss2")
            {
                type = EnemyType::BOSS2;
                range = 30;
            }
            else if (enemyTypeStr == "heal")
            {
                type = EnemyType::HEAL;
                range = 30;
            }
        }

        // function to check if a bullet should be spawned
        bool checkRateOfFire()
        {
            // get current weapon BPS
            // check for it's cooldown
            if (deltasCounter == 0)
            {
                deltasCounter++;
                return true;
            }
            else if ((deltasCounter) * 0.008335638028169f >= (1 / rateOfFire))
            {
                deltasCounter = 0;
            }
            else
            {
                deltasCounter++;
            }
            return false;
        }

        // function to rotate the enemy to point to the player
        void aimAt(CameraComponent *camera, bool isMelee = false)
        {
            glm::vec3 direction = -glm::normalize(this->getOwner()->localTransform.position - camera->getOwner()->localTransform.position);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Assuming Y-axis is up
            glm::quat rotation = glm::quatLookAt(direction, up);
            glm::vec3 rotationDegrees = glm::degrees(glm::eulerAngles(rotation));
            if (cos(glm::radians(rotationDegrees.x)) < 0)
            {
                rotationDegrees.y *= -1; // Player is behind, turn around
            }
            if(rotationDegrees.x>=90)
                rotationDegrees.x *=-1;
            

            if (isMelee)
            {
                getOwner()->localTransform.rotation.x = 0;
                getOwner()->localTransform.rotation.z = 0;
                getOwner()->localTransform.rotation.y = glm::radians(rotationDegrees[1]);
            }
            else
                getOwner()->localTransform.rotation = glm::radians(rotationDegrees);
        }

        void moveTowardsTarget(Entity *camera, float speed, float deltaTime,bool boss=false)
        {
            // Calculate direction vector from moving object to target object
            glm::vec3 direction = -normalize(this->getOwner()->localTransform.position - camera->localTransform.position);
            float zigzagSpeed = (rand() % 5 + 5) * deltaTime; // Adjust speed for the current frame

            if (zigZag)
            {
                if (fabs(direction.x) < fabs(direction.z))
                {
                    this->getOwner()->localTransform.position.x += direction.x * zigzagSpeed;
                    this->getOwner()->localTransform.position.z += direction.z * zigzagSpeed / 5;
                }
                else
                {
                    this->getOwner()->localTransform.position.z += direction.z * zigzagSpeed;
                    this->getOwner()->localTransform.position.x += direction.x * zigzagSpeed / 5;
                }
            }
            else
            {
                if (fabs(direction.x) < fabs(direction.z))
                {
                    this->getOwner()->localTransform.position.z += direction.z * zigzagSpeed;
                    this->getOwner()->localTransform.position.x -= direction.x * zigzagSpeed;
                }
                else
                {
                    this->getOwner()->localTransform.position.x += direction.x * zigzagSpeed;
                    this->getOwner()->localTransform.position.z -= direction.z * zigzagSpeed;
                }
            }
            timeSinceLastInversion += deltaTime;

            // Check if it's time to invert the zigzag pattern
            if (timeSinceLastInversion >= inversionInterval)
            {
                zigZag = !zigZag;              // Invert the zigzag pattern
                timeSinceLastInversion = 0.0f; // Reset the timer
            }
            if(boss){
                this->getOwner()->localTransform.position.y += direction.y * zigzagSpeed;

            }
            this->getOwner()->getComponents<ColliderComponent>()[0]->setEntity(this->getOwner());
            this->getOwner()->getComponents<ColliderComponent>()[1]->setEntity(this->getOwner());
        }

        bool isPlayerInRange(CameraComponent *player)
        {
            glm::vec3 playerPosition = player->getOwner()->localTransform.position;
            // TODO: check player collision

            return true;
        }
    };
};
