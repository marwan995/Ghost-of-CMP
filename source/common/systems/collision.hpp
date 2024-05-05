#pragma once

#include "../ecs/world.hpp"
#include "../components/collision-component.hpp"

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
    // class LaserBullet;

    // The collision system is responsible for checking collisions between colliders.
    class CollisionSystem
    {
        std::vector<Entity *> *staticEntities;  // to contain the static objects that don't move
        std::vector<Entity *> *dynamicEntities; // to contain the dynamic objects (camera and bullets)
        CameraComponent *camera;
        FreeCameraControllerComponent *cameraController;

        // Utility to remove a bullet
        void removeBullet(World *world, std::vector<Entity *>::iterator *dynamicIt)
        {
            world->markForRemoval((*(*dynamicIt)));
            auto dynamicIt2Delete = (*dynamicIt);
            (*dynamicIt) = (*dynamicIt) - 1;
            dynamicEntities->erase(dynamicIt2Delete);
        }

    public:
        // Only called when the play state starts to add the colliders in an array
        void enter(World *world)
        {
            // get access to the needed entities
            staticEntities = &world->staticEntities;
            dynamicEntities = &world->dynamicEntities;

            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                std::vector<ColliderComponent*> colliders = entity->getComponents<ColliderComponent>();

                CameraComponent *worldCamera = entity->getComponent<CameraComponent>();
                if (worldCamera)
                {
                    camera = worldCamera;
                    cameraController = entity->getComponent<FreeCameraControllerComponent>();
                }

                // If the movement component exists
                if (colliders.size()>0)
                {
                    for (auto collider : colliders)
                    {
                        collider->setEntity(entity);

                        // Change the position and rotation based on the linear & angular velocity and delta time.
                        if (collider->type == ColliderType::STATIC)
                        {
                            staticEntities->push_back(entity);
                        }
                        else
                        {
                            dynamicEntities->push_back(entity);
                        }
                    }
                }
            }
        }

        // function that runs in each frame to check for collisions
        void update(World *world, float deltaTime)
        {
            // used to get the new iterator of the rocket bullet when an explosion is generated
            int counter = 0;

            // For each dynamic entity in the world
            for (auto dynamicIt = dynamicEntities->begin(); dynamicIt != dynamicEntities->end(); dynamicIt++)
            {
                // flag to know if the bullet should be removed
                bool bulletVanished = false;

                // For each static entity in the world
                for (auto staticIt = staticEntities->begin(); staticIt != staticEntities->end(); staticIt++)
                {
                    auto staticComponent = (*staticIt)->getComponent<ColliderComponent>();
                    glm::vec3 collisionDepth = (*dynamicIt)->getComponent<ColliderComponent>()->collisionDepth(staticComponent);

                    if (ColliderComponent::isColliding(collisionDepth)) // if there's a collision
                    {
                        if ((*dynamicIt)->getComponent<CameraComponent>()) // camera collided with static object (wall)
                        {
                            auto staticComponentPostion = glm::vec3(staticComponent->x, staticComponent->y, staticComponent->z);

                            if (fabs(collisionDepth[1]) > fabs(staticComponentPostion[1]))
                            {
                                camera->getOwner()->localTransform.position[1] = camera->lastPosition[1];
                                cameraController->verticalVelocity = 0.0f;
                            }
                            else
                                camera->getOwner()->localTransform.position = camera->lastPosition;
                            break; // check for other collisions
                        }

                        // if it's not a camera then it's a projectile
                        ColliderComponent *projectileCollider = (*dynamicIt)->getComponent<ColliderComponent>();
                        // check if it's a bullet
                        if (projectileCollider->type == ColliderType::BULLET)
                        {
                            // flag to know if the enemy is killed
                            bool isKilled = false;

                            // get component for bullet type
                            our::LaserBullet *laser = (*dynamicIt)->getComponent<our::LaserBullet>();       // possible laser bullet component
                            our::ShotgunBullet *shotgun = (*dynamicIt)->getComponent<our::ShotgunBullet>(); // possible shotgun bullet component
                            our::RocketBullet *rocket = (*dynamicIt)->getComponent<our::RocketBullet>();    // possible rocket component
                            our::Explosion *explosion = (*dynamicIt)->getComponent<our::Explosion>();       // possible explosion component
                            if (laser)
                            {
                                // laser is always removed on collision
                                bulletVanished = true;

                                if (laser->isFriendly)                         // player's bullet
                                    isKilled = laser->hit(world, (*staticIt)); // apply damage & check if enemy is killed
                                // else
                                // std::cout<<"FUCK U U KILLED ME \n";                                                              // enemy's bullet
                            }
                            else if (shotgun)
                            {
                                // shotgun bullets don't vanish on collision they vanish after a certain time
                                if (shotgun->isFriendly)                         // player's bullet
                                    isKilled = shotgun->hit(world, (*staticIt)); // apply damage & check if enemy is killed

                                // else                                                                  // enemy's bullet
                            }
                            else if (rocket)
                            {
                                bulletVanished = true;

                                if (rocket->isFriendly)                         // player's bullet
                                    isKilled = rocket->hit(world, (*staticIt)); // apply damage & check if enemy is killed

                                // else                                                                  // enemy's bullet

                                dynamicIt = dynamicEntities->begin() + counter;
                            }
                            else if (explosion)
                            {
                                if (explosion->isFriendly)                         // player's bullet
                                    isKilled = explosion->hit(world, (*staticIt)); // apply damage & check if enemy is killed

                                // else                                                                  // enemy's bullet
                            }

                            // hit entity is killed so remove it
                            if (isKilled)
                            {
                                staticEntities->erase(staticIt);
                            }

                            // bullet isn't removed in case of shotgun & explosion
                            if (bulletVanished)
                            {
                                // remove the bullet & it's collider
                                removeBullet(world, &dynamicIt);
                                break; // the bullet vanished so check next dynamic collider
                            }
                        }
                    }

                    if (staticEntities->size() == 0)
                    {
                        break;
                    }
                }

                if (dynamicEntities->size() == 1)
                {
                    break;
                }

                counter++;
            }
        }

        // function to remove shotgun bullets and explosions after a certain time
        void updateBullets(World *world)
        {
            for (auto dynamicIt = dynamicEntities->begin(); dynamicIt != dynamicEntities->end(); dynamicIt++)
            {
                ShotgunBullet *shotgun = (*dynamicIt)->getComponent<ShotgunBullet>();
                Explosion *explosion = (*dynamicIt)->getComponent<Explosion>();
                if (shotgun)
                {
                    if (shotgun->checkBulletRemoval())
                    {
                        removeBullet(world, &dynamicIt);
                    }
                }
                else if (explosion)
                {
                    if (explosion->checkBulletRemoval())
                    {
                        removeBullet(world, &dynamicIt);
                    }
                }

                if (dynamicEntities->size() == 1)
                {
                    return;
                }
            }
        }
    };
}
