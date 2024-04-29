#pragma once

#include "../ecs/world.hpp"
#include "../components/collision-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "../components/camera.hpp"

namespace our
{

    // The collision system is responsible for checking collisions between colliders. 
    class CollisionSystem {
        std::vector<Entity*> staticEntities;    // to contain the static objects that don't move
        std::vector<Entity*> dynamicEntities;   // to contain the dynamic objects (camera and bullets)
        CameraComponent* camera;
        
        ColliderComponent* getCollider(Entity* entity){
            ColliderComponent* collider = entity->getComponent<ColliderComponent>();
            return collider? collider : NULL;
        } 

    public:

        // Only called when the play state starts to add the colliders in an array 
        void enter(World* world) {
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
                        staticEntities.push_back(entity);
                    }
                    else{
                        dynamicEntities.push_back(entity);
                    }
                }
            }
        }

        //
        void update(World* world, float deltaTime) {
            // For each entity in the world
            for(auto dynamicEntity : dynamicEntities)
            {
                // CameraComponent* camera = world->getEntities()->getComponent<CameraComponent>(0);
                for (auto staticEntity : staticEntities)
                {
                    glm::vec3 collisionDepth = dynamicEntity->getComponent<ColliderComponent>()->collisionDepth(staticEntity->getComponent<ColliderComponent>());
                    // TODO: continue here
                    if (ColliderComponent::isColliding(collisionDepth))
                    {
                        if (dynamicEntity->getComponent<CameraComponent>())
                        {
                            // camera collided with wall
                            camera->getOwner()->localTransform.position = camera->lastPosition;
                            world->markForRemoval(staticEntity);
                            auto it = find(staticEntities.begin(),staticEntities.end(),staticEntity);
                            staticEntities.erase(it);
                        }
                            std::cout<<"collision\n";
                    }
                }
            }
        }

    };

}
