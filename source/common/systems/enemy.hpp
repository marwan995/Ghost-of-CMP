#pragma once

#include "../ecs/world.hpp"
#include "../components/enemy-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "../components/camera.hpp"

// bullet types
#include "../ecs/laser-bullet.hpp"
#include "../ecs/shotgun-bullet.hpp"
#include "../ecs/rocket-bullet.hpp"
#include "../ecs/explosion.hpp"

namespace our
{
    // The Enemy system is responsible for attacking the player.
    class EnemySystem
    {
        CameraComponent *camera;
        std::vector<Entity *> *enemiesEntities;

    public:
        // get the needed entities
        void enter(World *world)
        {
            // get access to the needed entities
            enemiesEntities = &world->enemiesEntities;

            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                EnemyComponent *enemy = entity->getComponent<EnemyComponent>();

                // Get the camera component if it exists
                CameraComponent *worldCamera = entity->getComponent<CameraComponent>();
                if (worldCamera)
                {
                    camera = worldCamera;
                }

                // If the enemy component exists
                if (enemy)
                {
                    enemiesEntities->push_back(entity);
                }
            }
        }

        // check if the player is in range
        void update(World *world, double deltaTime)
        {
            // get the camera collider
            ColliderComponent *cameraCollider = camera->getOwner()->getComponent<ColliderComponent>();
            // For each entity in the world
            for (auto entity : *enemiesEntities)
            {
                // Get the enemy component if it exists
                EnemyComponent *enemy = entity->getComponent<EnemyComponent>();

                // If the enemy component exists
                if (enemy)
                {

                    ColliderComponent *enemyCollider = entity->getComponents<ColliderComponent>()[1];
                    if (!enemyCollider)
                    {
                        break;
                    }
                    glm::vec3 collisionDepth = enemyCollider->collisionDepth(cameraCollider);

                    if (ColliderComponent::isColliding(collisionDepth))
                    { 
                        Boss1* testBoss = enemy->getChild<Boss1>();
                        if (testBoss)
                        {
                            testBoss->action();
                        }
                        if (enemy->type == EnemyType::MELEE)
                        {
                            enemy->aimAt(camera,true);
                            enemy->moveTowardsTarget(camera->getOwner(),deltaTime,deltaTime);
                        }
                        else if (enemy->type == EnemyType::SHOOTER)
                        {
                            enemy->aimAt(camera);
                            if (enemy->checkRateOfFire())
                            {
                                glm::vec3 rotation = enemy->getOwner()->localTransform.rotation;
                                glm::vec3 position = enemy->getOwner()->localTransform.position;

                                float bulletRotation[3] = {180 - glm::degrees(rotation.x), glm::degrees(rotation.y) - 180, glm::degrees(rotation.z)};
                                float bulletSpeedX = -cos(-rotation.x) * sin(rotation.y);
                                float bulletSpeedY = -sin(-rotation.x);
                                float bulletSpeedZ = -cos(-rotation.x) * cos(rotation.y);
                                float bulletMovementDirections[3] = {bulletSpeedX, bulletSpeedY, bulletSpeedZ};
                                float bulletPosition[3] = {position.x + bulletSpeedX * 2, position.y + bulletSpeedY * 2, position.z + bulletSpeedZ * 2};

                                LaserBullet *laserBullet = new LaserBullet(bulletPosition, bulletRotation, bulletMovementDirections, world, false);
                                laserBullet->shoot();
                            }
                        }
                    }
                }
            }
        }

        void enemyKilled(Entity *killedEntity)
        {
            auto it = std::find(enemiesEntities->begin(), enemiesEntities->end(), killedEntity);
            enemiesEntities->erase(it);
        }
    };
}