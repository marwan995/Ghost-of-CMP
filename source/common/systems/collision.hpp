#pragma once

#include "../ecs/world.hpp"
#include "../components/collision-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The collision system is responsible for checking collisions between colliders. 
    class CollisionSystem {
        std::vector<Entity*> staticColliders;    // to contain the static objects that don't move
        std::vector<Entity*> dynamicColliders;   // to contain the dynamic objects (camera and bullets)
        
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
                // If the movement component exists
                if(collider){
                    // Change the position and rotation based on the linear & angular velocity and delta time.
                    if(collider->type == ColliderType::STATIC)
                    {
                        staticColliders.push_back(entity);
                    }
                    else{
                        dynamicColliders.push_back(entity);
                    }
                }
            }
        }

        //
        void update(World* world, float deltaTime) {
            // For each entity in the world
            for(auto dynamicCollider : dynamicColliders)
            {
                for (auto staticCollider : staticColliders)
                {
                    // TODO: continue here
                    if (ColliderComponent::isColliding(dynamicCollider->collisionDepth(staticCollider)))
                    {
                        std::cout<<"We did it\n";
                    }
                }
            }
        }

    };

}
