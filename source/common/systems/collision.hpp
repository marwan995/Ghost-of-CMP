#pragma once

#include "../ecs/world.hpp"
#include "../components/collision-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "../components/camera.hpp"
#include "../ecs/laser-bullet.hpp"

namespace our
{
    // class LaserBullet;

    // The collision system is responsible for checking collisions between colliders. 
    class CollisionSystem {
        std::vector<Entity*>* staticEntities;    // to contain the static objects that don't move
        std::vector<Entity*>* dynamicEntities;   // to contain the dynamic objects (camera and bullets)
        CameraComponent* camera;
        
        ColliderComponent* getCollider(Entity* entity){
            ColliderComponent* collider = entity->getComponent<ColliderComponent>();
            return collider? collider : NULL;
        }

    public:

        // mainly used to add bullets
        // static void addDynamicEntity(Entity* newEntity){
        //     dynamicEntities->push_back(newEntity);
        // }

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

        // runs in each frame
        void update(World* world, float deltaTime) {
            // For each dynamic entity in the world
            // TODO: make the loop use iterators
            for(auto dynamicIt = dynamicEntities->begin(); dynamicIt != dynamicEntities->end(); dynamicIt++)
            {
                for (auto staticIt = staticEntities->begin(); staticIt != staticEntities->end(); staticIt++)
                {
                    glm::vec3 collisionDepth = (*dynamicIt)->getComponent<ColliderComponent>()->collisionDepth((*staticIt)->getComponent<ColliderComponent>());
                    
                    if (ColliderComponent::isColliding(collisionDepth))     // if there's a collision
                    {
                        if ((*dynamicIt)->getComponent<CameraComponent>())      // camera collided with static object (wall)
                        {
                            // keep the camera position
                            camera->getOwner()->localTransform.position = camera->lastPosition;
                            break;      // check for other collisions
                        }

                        ColliderComponent* possibleBullet = (*dynamicIt)->getComponent<ColliderComponent>();
                        if (possibleBullet->type == ColliderType::BULLET)           // check if it's a bullet
                        {
                            bool isKilled = false;

                            our::LaserBullet* laser = (*dynamicIt)->getComponent<our::LaserBullet>();   // if the bullet is a laser
                            if (laser)
                            {
                                // if (laser->isFriendly)                                                  // player's bullet
                                    isKilled = laser->hit(world, (*dynamicIt),(*staticIt));             // apply damage & check if enemy is killed
                                //else                                                                  // enemy's bullet
                            }
                            // TODO: other types of bullets
                            // else if ()
                            // {

                            // }

                            if (isKilled)
                            {
                                // hit entity is killed so remove it
                                staticEntities->erase(staticIt);
                            }
                        }

                        // remove the bullet & it's collider
                        world->markForRemoval((*dynamicIt));
                        auto dynamicIt2Delete = dynamicIt;
                        dynamicIt--;
                        dynamicEntities->erase(dynamicIt2Delete);
                        break; // the bullet vanished so check next dynamic collider
                    }
                }
            }
        }
    };
}
