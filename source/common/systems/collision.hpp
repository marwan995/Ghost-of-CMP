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
                bool dynamicErased = false;
                
                for (auto staticIt = staticEntities->begin(); staticIt != staticEntities->end(); staticIt++)
                {
                    glm::vec3 collisionDepth = (*dynamicIt)->getComponent<ColliderComponent>()->collisionDepth((*staticIt)->getComponent<ColliderComponent>());
                    if (ColliderComponent::isColliding(collisionDepth))
                    {
                        if ((*dynamicIt)->getComponent<CameraComponent>())
                        {
                            // camera collided with wall
                            camera->getOwner()->localTransform.position = camera->lastPosition;
                            world->markForRemoval((*staticIt));
                            staticEntities->erase(staticIt);
                            return;
                        }

                        // TODO: a way to access the bullet from the entity
                        // then check if it's enemy or ally
                        ColliderComponent* possibleBullet = (*dynamicIt)->getComponent<ColliderComponent>();
                        if (possibleBullet->type == ColliderType::BULLET)
                        {
                            bool isKilled = false;
                            our::LaserBullet* laser = (*dynamicIt)->getComponent<our::LaserBullet>();
                            if (laser)
                            {
                                isKilled = laser->hit((*staticIt));
                            }
                            // check if it's friendly fire

                            // hit entity is killed so remove it
                            if (isKilled)
                            {
                                world->markForRemoval((*staticIt));
                                world->markForRemoval((*dynamicIt));
                                staticEntities->erase(staticIt);
                                dynamicErased = true;
                            }

                            break; // the bullet and the static object vanished
                        }
                    }
                }

                // if a bullet hit something delete it
                if (dynamicErased)
                {
                    auto dynamicIt2Delete = dynamicIt;
                    dynamicIt--;
                    dynamicEntities->erase(dynamicIt2Delete);
                }
            }
        }

    };


}
