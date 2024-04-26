#pragma once

#include "../ecs/world.hpp"
#include "../components/collision-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class CollisionSystem {
        std::vector<ColliderComponent*> staticColliders;
        std::vector<ColliderComponent*> dynamicColliders;

    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
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
                        staticColliders.push_back(collider);
                    }
                    else{
                        dynamicColliders.push_back(collider);
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
                    if (ColliderComponent::isColliding(dynamicCollider->collisionDepth(staticCollider)))
                    {
                        std::cout<<"We did it\n";
                    }
                }
            }
        }

    };

}
