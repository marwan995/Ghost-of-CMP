#pragma once

#include "../ecs/world.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

// #include "../components/camera.hpp"
// #include "../components/free-camera-controller.hpp"

namespace our{
    // this class is responsible for unlocking the final boss room
    // and unlocking weapons
    class UnlockSystem
    {
        std::map<std::string, float>* weapons;              // reference to the player's weapons map
        glm::vec3* position;                       // reference to the player position
        bool isBoss1Killed = false;                         // the state of the first boss (to unlock the second)

        // UTILITY to check if the player gained the shotgun
        void checkShotgunUnlock()
        {
            // TODO: check player position
            // in the right position & shotgun isn't added yet
            if (weapons->find("shotgun") == weapons->end() && position->x <= -26.5 && position->z >= -10.5 && position->z <= -7.5)
            {
                std::cout<<"added shotgun\n";
                weapons->insert({"shotgun", 5});            // add shotgun & its rate of fire
            }
            // TODO: remove the weapon from the world
        }

        // UTILITY to check if the player gained the rocket launcher
        void checkRocketUnlock()
        {
            // TODO: check player position
            if (false)
            {
                weapons->insert({"rocket", 15});            // add rocket launcher & its rate of fire
            }

            // TODO: remove the weapon from the world
        }
        
        public:

        // take a reference to the player position, weapons
        // returns a reference to the state of the first boss (killed or alive)
        bool* enter(std::map<std::string, float>* playerWeapons, glm::vec3* playerPosition)
        {
            weapons = playerWeapons;
            position = playerPosition;
            return & isBoss1Killed;
        }

        void update()
        {
            checkShotgunUnlock();
            checkRocketUnlock();
        }

    };
}