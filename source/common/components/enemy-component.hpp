#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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
        BOSS2
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
                range = 30;
            }
            else if (enemyTypeStr == "boss2")
            {
                type = EnemyType::BOSS2;
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
            auto M = getOwner()->getLocalToWorldMatrix();
            glm::vec3 eye = glm::vec3(M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            glm::vec3 up = glm::vec3(M * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
            glm::mat4 matrix = glm::lookAt(eye, camera->getOwner()->localTransform.position, up);

            glm::quat newRotation;
            glm::vec3 temp;
            glm::vec4 temp4;

            glm::decompose(matrix, temp, newRotation, temp, temp, temp4);

            glm::vec3 rotationDegrees = glm::degrees(glm::eulerAngles(newRotation));
            
            // if player is behind the enemy
            if (cos(glm::radians(rotationDegrees.x)) > 0)
            {
                // as the result is ranging from -PI/2 to PI/2 (strange)
                rotationDegrees.y *= -1;
                // z axis to prevent wiggling
                rotationDegrees.z = 0;
            }
            else
            {
                rotationDegrees.z = -180;
            }

            rotationDegrees.x *= -1;
            if (isMelee)
            {
                getOwner()->localTransform.rotation.x = 0;
                getOwner()->localTransform.rotation.z = 0;
                getOwner()->localTransform.rotation.y = glm::radians(rotationDegrees[1] - 90);
            }
            else
                getOwner()->localTransform.rotation = glm::radians(rotationDegrees);
        }


        void moveTowardsTarget(Entity *camera, float speed, float deltaTime)
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
                else{
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
            this->getOwner()->getComponents<ColliderComponent>()[0]->setEntity(this->getOwner());
            this->getOwner()->getComponents<ColliderComponent>()[1]->setEntity(this->getOwner());
        }
        void moveCircular(Entity *camera, float radius, float angularSpeed, float deltaTime)
        {
            // Calculate direction vector from moving object to target object
            glm::vec3 direction = -normalize(this->getOwner()->localTransform.position - camera->localTransform.position);
            // Calculate position on the circle
            float angle = 5 * deltaTime;
            glm::vec3 offset = glm::vec3(10 * cos(angle), 0.0f, 10 * sin(angle));
            // Adjust position of moving object based on direction and speed
            this->getOwner()->localTransform.position[0] += direction[0] + offset[0];
            this->getOwner()->localTransform.position[1] += direction[1] + offset[1];
            this->getOwner()->localTransform.position[2] += direction[2] + offset[2];
        }
        bool isPlayerInRange(CameraComponent *player)
        {
            glm::vec3 playerPosition = player->getOwner()->localTransform.position;
            // TODO: check player collision

            return true;
        }
    };
};
