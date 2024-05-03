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
    class CollisionSystem {
        std::vector<Entity*>* staticEntities;    // to contain the static objects that don't move
        std::vector<Entity*>* dynamicEntities;   // to contain the dynamic objects (camera and bullets)
        CameraComponent* camera;

        // Utility to remove a bullet
        void removeBullet(World* world, std::vector<Entity*>::iterator* dynamicIt)
        {
            world->markForRemoval((*(*dynamicIt)));
            auto dynamicIt2Delete = (*dynamicIt);
            (*dynamicIt) = (*dynamicIt) - 1;
            dynamicEntities->erase(dynamicIt2Delete);
        }

    public:

        // Only called when the play state starts to add the colliders in an array 
        void enter(World* world) {
            // get access to the needed entities
            staticEntities = &world->staticEntities;
            dynamicEntities = &world->dynamicEntities;

            // For each entity in the world
            for(auto entity : world->getEntities()){
                // Get the movement component if it exists
                ColliderComponent* collider = entity->getComponent<ColliderComponent>();

                CameraComponent* worldCamera = entity->getComponent<CameraComponent>();
                if (worldCamera)
                {
                    camera = worldCamera;
                }

                // If the movement component exists
                if(collider){
                    collider->setEntity(entity);

                    // Change the position and rotation based on the linear & angular velocity and delta time.
                    if(collider->type == ColliderType::STATIC)
                    {
                        staticEntities->push_back(entity);
                    }
                    else{
                        dynamicEntities->push_back(entity);
                    }
                }
            }
        }

        // function that runs in each frame to check for collisions
        void update(World* world, float deltaTime) {
            // For each dynamic entity in the world
            for(auto dynamicIt = dynamicEntities->begin(); dynamicIt != dynamicEntities->end(); dynamicIt++)
            {   
                // For each static entity in the world
                for (auto staticIt = staticEntities->begin(); staticIt != staticEntities->end(); staticIt++)
                {
                    glm::vec3 collisionDepth = (*dynamicIt)->getComponent<ColliderComponent>()->collisionDepth((*staticIt)->getComponent<ColliderComponent>());
                    
                    // if there's a collision
                    if (ColliderComponent::isColliding(collisionDepth))
                    {
                        // camera collided with static object (wall)
                        if ((*dynamicIt)->getComponent<CameraComponent>())
                        {
                            // keep the camera position
                            camera->getOwner()->localTransform.position = camera->lastPosition;
                            break;      // check for other collisions
                        }

                        // if it's not a camera then it's a projectile
                        ColliderComponent* projectileCollider = (*dynamicIt)->getComponent<ColliderComponent>();
                        // check if it's a bullet
                        if (projectileCollider->type == ColliderType::BULLET)
                        {
                            // flag to know if the enemy is killed
                            bool isKilled = false;
                            // flag to know if the bullet should be removed
                            bool bulletVanished = false;

                            // get component for bullet type
                            our::LaserBullet* laser = (*dynamicIt)->getComponent<our::LaserBullet>();   // possible laser bullet component
                            our::ShotgunBullet* shotgun = (*dynamicIt)->getComponent<our::ShotgunBullet>();         // possible shotgun bullet component
                            our::RocketBullet* rocket = (*dynamicIt)->getComponent<our::RocketBullet>();         // possible rocket component
                            our::Explosion* explosion = (*dynamicIt)->getComponent<our::Explosion>();         // possible explosion component
                            if (laser)
                            {
                                // laser is always removed on collision
                                bulletVanished = true;

                                if (laser->isFriendly)                                                  // player's bullet
                                    isKilled = laser->hit(world, (*dynamicIt),(*staticIt));             // apply damage & check if enemy is killed
                                //else                                                                  // enemy's bullet
                            }
                            else if (shotgun)
                            {
                                // shotgun bullets don't vanish on collision they vanish after a certain time
                                if (shotgun->isFriendly)                                                  // player's bullet
                                    isKilled = shotgun->hit(world, (*dynamicIt),(*staticIt));             // apply damage & check if enemy is killed
                                
                                //else                                                                  // enemy's bullet
                            }
                            else if (rocket)
                            {
                                bulletVanished = true;
                                
                                if (rocket->isFriendly)                                                  // player's bullet
                                    isKilled = rocket->hit(world, (*dynamicIt),(*staticIt));             // apply damage & check if enemy is killed
        
                                //else                                                                  // enemy's bullet

                            }
                            else if (explosion)
                            {
                                if (explosion->isFriendly)                                                  // player's bullet
                                    isKilled = explosion->hit(world, (*dynamicIt),(*staticIt));             // apply damage & check if enemy is killed
        
                                //else                                                                  // enemy's bullet
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
                }
            }
        }

        // function to remove shotgun bullets and explosions after a certain time
        void updateBullets(World* world)
        {
            for(auto dynamicIt = dynamicEntities->begin(); dynamicIt != dynamicEntities->end(); dynamicIt++)
            {
                ShotgunBullet* shotgun = (*dynamicIt)->getComponent<ShotgunBullet>();
                Explosion* explosion = (*dynamicIt)->getComponent<Explosion>();
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
            }
        }
    };
}
