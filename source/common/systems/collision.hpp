#pragma once

#include "../ecs/world.hpp"
#include "../components/collision-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

// camera that represents player
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

// enemy system to make them attack player
#include "enemy.hpp"

// bullet types
#include "../ecs/laser-bullet.hpp"
#include "../ecs/shotgun-bullet.hpp"
#include "../ecs/rocket-bullet.hpp"
#include "../ecs/explosion.hpp"

// declaration for the function pointer type 
typedef void (*CallbackFunction)(our::CameraComponent *, float);

namespace our
{
    // class LaserBullet;

    // The collision system is responsible for checking collisions between colliders.
    class CollisionSystem
    {
        std::vector<Entity *> *staticEntities;  // to contain the static objects that don't move
        std::vector<Entity *> *dynamicEntities; // to contain the dynamic objects (camera and bullets)
        std::vector<Entity *> *enemiesEntities; // to contain the dynamic objects (camera and bullets)
        EnemySystem *enemySys;
        CameraComponent *camera;
        FreeCameraControllerComponent *cameraController;
        CallbackFunction reducePlayerHealthCallBack;

        bool isBoss1Killed = false;               // flag to control the navigation to boss 2 using unlock system
        bool isBoss2Killed = false;               // flag to know the player has won

        void removeEntityFromVector(Entity *entityToRemove, std::vector<Entity *> &entitiesVector)
        {
            entitiesVector.erase(std::remove_if(entitiesVector.begin(), entitiesVector.end(),
                                                [entityToRemove](Entity *entity)
                                                {
                                                    return entity == entityToRemove;
                                                }),
                                 entitiesVector.end());
        }

        // Utility to remove a bullet
        void removeBullet(World *world, std::vector<Entity *>::iterator *dynamicIt)
        {
            world->markForRemoval((*(*dynamicIt)));
            auto dynamicIt2Delete = (*dynamicIt);
            (*dynamicIt) = (*dynamicIt) - 1;
            dynamicEntities->erase(dynamicIt2Delete);
        }
        
        // Utility to remove an enemy
        void removeEnemy(World *world, Entity *enemy)
        {
            world->markForRemoval(enemy);
            removeEntityFromVector(enemy, *dynamicEntities);
            removeEntityFromVector(enemy, *staticEntities);
            removeEntityFromVector(enemy, *(enemiesEntities));
        }

        // Utility to check the bullet type and apply damage
        bool checkBulletType(
                            World* world,
                            our::LaserBullet* laser,
                            our::ShotgunBullet* shotgun,
                            our::RocketBullet* rocket,
                            our::Explosion* explosion,
                            bool* bulletVanished,
                            std::vector<our::Entity *>::iterator* dynamicIt,
                            std::vector<our::Entity *>::iterator* staticIt,
                            int* counter
                            )
        {
            bool isEnemyKilled = false;
            if (laser)
            {
                // laser is always removed on collision
                (*bulletVanished) = true;

                if (laser->isFriendly)                         // player's bullet
                    isEnemyKilled = laser->hit(world, (*(*staticIt))); // apply damage & check if enemy is killed
                else
                    reducePlayerHealthCallBack(camera, 5);
            }
            else if (shotgun)
            {
                // shotgun bullets don't vanish on collision they vanish after a certain time
                if (shotgun->isFriendly)                         // player's bullet
                    isEnemyKilled = shotgun->hit(world, (*(*staticIt))); // apply damage & check if enemy is killed
                else                                             // enemy's bullet
                    reducePlayerHealthCallBack(camera, 50);
            }
            else if (rocket)
            {
                (*bulletVanished) = true;

                if (rocket->isFriendly)                         // player's bullet
                    isEnemyKilled = rocket->hit(world, (*(*staticIt))); // apply damage & check if enemy is killed
                else                                            // enemy's bullet
                    reducePlayerHealthCallBack(camera, 200);
                (*dynamicIt) = dynamicEntities->begin() + (*counter);
            }
            else if (explosion)
            {
                if (explosion->isFriendly)                         // player's bullet
                    isEnemyKilled = explosion->hit(world, (*(*staticIt))); // apply damage & check if enemy is killed
                else                                               // enemy's bullet
                    reducePlayerHealthCallBack(camera, 100);
            }

            return isEnemyKilled;
        }


    public:
        // Only called when the play state starts to add the colliders in an array
        void enter(World *world, EnemySystem *enemySystem, CallbackFunction reduceHealth)
        {
            // get access to the needed entities
            staticEntities = &world->staticEntities;
            dynamicEntities = &world->dynamicEntities;
            enemiesEntities = &world->enemiesEntities;
            reducePlayerHealthCallBack = reduceHealth;

            enemySys = enemySystem;

            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                std::vector<ColliderComponent *> colliders = entity->getComponents<ColliderComponent>();

                CameraComponent *worldCamera = entity->getComponent<CameraComponent>();
                if (worldCamera)
                {
                    camera = worldCamera;
                    cameraController = entity->getComponent<FreeCameraControllerComponent>();
                }

                // If the movement component exists
                if (colliders.size() > 0)
                {
                    for (auto collider : colliders)
                    {
                        collider->setEntity(entity);
                    }
                    if (colliders[0]->type == ColliderType::STATIC)
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

        // function that runs in each frame to check for collisions
        void update(World *world)
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
                    EnemyComponent* enemy = (*staticIt)->getComponent<EnemyComponent>();
                    bool isBoss1 = false;
                    bool isBoss2 = false;
                    if (enemy)
                    {
                        if (enemy->type == EnemyType::BOSS1)
                            isBoss1 = true;
                        else if (enemy->type == EnemyType::BOSS2)
                            isBoss2 = true;
                    }

                    auto staticComponent = (*staticIt)->getComponent<ColliderComponent>();
                    glm::vec3 collisionDepth = (*dynamicIt)->getComponent<ColliderComponent>()->collisionDepth(staticComponent);

                    if (ColliderComponent::isColliding(collisionDepth)) // if there's a collision
                    {
                        if ((*dynamicIt)->getComponent<CameraComponent>()) // camera collided with static object (wall)
                        {
                            auto staticComponentPostion = glm::vec3(staticComponent->x, staticComponent->y, staticComponent->z);
                            if (enemy)
                            {
                                if (enemy->type == EnemyType::MELEE)
                                {
                                    reducePlayerHealthCallBack(camera, 60);
                                    removeEnemy(world, enemy->getOwner());
                                }
                            }
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

                            // get component for bullet type
                            our::LaserBullet *laser = (*dynamicIt)->getComponent<our::LaserBullet>();       // possible laser bullet component
                            our::ShotgunBullet *shotgun = (*dynamicIt)->getComponent<our::ShotgunBullet>(); // possible shotgun bullet component
                            our::RocketBullet *rocket = (*dynamicIt)->getComponent<our::RocketBullet>();    // possible rocket component
                            our::Explosion *explosion = (*dynamicIt)->getComponent<our::Explosion>();       // possible explosion component
                            
                            // check the bullet type to determine if the enemy is killed
                            // or if the player health should be reduced
                            bool isEnemyKilled = checkBulletType(
                                                    world,
                                                    laser,
                                                    shotgun,
                                                    rocket,
                                                    explosion,
                                                    &bulletVanished,
                                                    &dynamicIt,
                                                    &staticIt,
                                                    &counter
                                );

                            // hit entity is killed so remove it
                            if (isEnemyKilled)
                            {
                                if ((*staticIt)->getComponent<EnemyComponent>())
                                    enemySys->enemyKilled((*staticIt));
                                staticEntities->erase(staticIt);
                                
                                // if the enemy was a boss handle its logic (end game or unlock boss2)
                                if (isBoss1)
                                    isBoss1Killed = true;
                                else if (isBoss2)
                                    isBoss2Killed = true;
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

                    // no more static colliders
                    if (staticEntities->size() == 0)
                    {
                        break;
                    }
                }

                // only the camera collider is remaining
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

        bool checkBoss1Killed()
        {
            return isBoss1Killed;
        }

        bool checkBoss2Killed()
        {
            return isBoss2Killed;
        }
    };
}
