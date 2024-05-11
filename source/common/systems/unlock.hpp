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
        bool isBoss1Killed;                 // the state of the first boss (to unlock the second)
        World* world;                               // reference to the world to mark entities for removal
        Entity* shotgunEntity;               // reference to the shotgun entity
        Entity* rocketEntity;                // reference to the rocket launcher entity
        Entity* boss2WallEntity;                // reference to boss 2 wall's entity

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

        // UTILITY to remove boss 2 wall when boss 1 is killed
        void checkBoss2Wall()
        {
            // if the wall exists and boss 1 is killed
            if (boss2WallEntity != NULL && isBoss1Killed)
            {
                world->markForRemoval(boss2WallEntity);
            }
        }
        
        public:

        // take a reference to the player position, weapons
        // returns a reference to the state of the first boss (killed or alive)
        bool* enter(World* currentWorld, std::map<std::string, float>* playerWeapons, glm::vec3* playerPosition)
        {
            isBoss1Killed = false;
            shotgunEntity = NULL;
            rocketEntity = NULL;
            boss2WallEntity = NULL;

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
                else if (entity->name == "boss 2 wall")
                    boss2WallEntity = entity;
                
                // so it doesn't loop over the whole world
                if (shotgunEntity != NULL && rocketEntity != NULL && boss2WallEntity != NULL)
                    break;
            }

            return & isBoss1Killed;
        }

        // called every frame to check for player/world changes
        void update()
        {
            checkShotgunUnlock();
            checkRocketUnlock();
            checkBoss2Wall();
        }

    };
}