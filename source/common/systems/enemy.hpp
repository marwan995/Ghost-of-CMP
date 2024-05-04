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
    // The collision system is responsible for checking collisions between colliders.
    class EnemySystem
    {

        public:
        void enter(World *world)
        {
            // TODO: initialize the enemies vector
               // get access to the needed entities
            enemiesEntities = &world->enemiesEntities;

            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                EnemyComponent *collider = entity->getComponent<ColliderComponent>();

                CameraComponent *worldCamera = entity->getComponent<CameraComponent>();
                if (worldCamera)
                {
                    camera = worldCamera;
                    cameraController = entity->getComponent<FreeCameraControllerComponent>();
                }

                // If the movement component exists
                if (collider)
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

        void update(World* world ,double deltaTime)
        {
            // TODO: loop through enemies and check if player is in collision then attack

        }

    };
}