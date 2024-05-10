#pragma once

#include "../ecs/world.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our{
    // this class is responsible for unlocking the final boss room
    // and unlocking weapons
    class UnlockSystem
    {
        std::map<std::string, float>* weapons;      // reference to the player's weapons map
        glm::vec3* position;                        // reference to the player position
        bool isBoss1Killed = false;                 // the state of the first boss (to unlock the second)
        World* world;                               // reference to the world to mark entities for removal
        Entity* shotgunEntity = NULL;               // reference to the shotgun entity
        Entity* rocketEntity = NULL;                // reference to the rocket launcher entity

        // UTILITY to check if the player gained the shotgun
        void checkShotgunUnlock()
        {
            // in the right position & shotgun isn't added yet
            if (weapons->find("shotgun") == weapons->end() && position->x <= -26.5 && position->z >= -10.5 && position->z <= -7.5)
            {
                // add shotgun & its rate of fire
                weapons->insert({"shotgun", 10});
                
                // remove the weapon from the world
                world->markForRemoval(shotgunEntity);
            }
        }

        // UTILITY to check if the player gained the rocket launcher
        void checkRocketUnlock()
        {
            // in the right position & rocket launcher isn't added yet
            if (weapons->find("rocket") == weapons->end() && position->x <= -61 && position->z >= -50.5 && position->z <= 48.1)
            {
                // add rocket launcher & its rate of fire
                weapons->insert({"rocket", 7});
                
                // remove the weapon from the world
                world->markForRemoval(rocketEntity);
            }
        }
        
        public:

        // take a reference to the player position, weapons
        // returns a reference to the state of the first boss (killed or alive)
        bool* enter(World* currentWorld, std::map<std::string, float>* playerWeapons, glm::vec3* playerPosition)
        {
            weapons = playerWeapons;
            position = playerPosition;
            world = currentWorld;

            for (auto entity: world->getEntities())
            {
                // get the shotgun & rocket entities to remove them when the weapon is aquired
                if (entity->name == "world shotgun")
                    shotgunEntity = entity;
                else if (entity->name == "world rocket")
                    rocketEntity = entity;
                
                // so it doesn't loop over the whole world
                if (shotgunEntity != NULL && rocketEntity != NULL)
                    break;
            }


            return & isBoss1Killed;
        }

        void update()
        {
            checkShotgunUnlock();
            checkRocketUnlock();
            // TODO: check if boss1 is killed
        }

    };
}