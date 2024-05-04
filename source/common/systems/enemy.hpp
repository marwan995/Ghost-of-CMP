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
    class CollisionSystem
    {
        std::vector<Entity*> enemiesVec;

        public:
        void enter(World *world)
        {
            // TODO: initialize the enemies vector
        }

        void update(World* world)
        {
            // TODO: loop through enemies and check if player is in collision then attack

        }

    }
}