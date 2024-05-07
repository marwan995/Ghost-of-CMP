#pragma once

#include "../components/projectile.hpp"
#include <GLFW/glfw3.h>         // to get the time

namespace our{
    class Explosion : public Projectile
    {
    private:
        World* world;
        double spawnTime;                       // the time at which the bullet is shot
        double lifeTime = 0.35f;          // the duration the bullet should stay for

    public:
        // initialize the bullet attributes
        Explosion(const float (&cameraPosition)[3], World* currentWorld, bool friendlyFire)
        {
            mesh = "Explosion";
            material = "Image_26";
            scale[0] = 0.3;
            scale[1] = 0.3;
            scale[2] = 0.3;
            spawnTime = glfwGetTime();
            
            world = currentWorld;
            speed = 0;
            radius = 2;
            damage = 400;
            isFriendly = friendlyFire;

            Projectile::copyArr(position, cameraPosition);
            // Projectile::copyArr(rotation, {0,0,0});
            // Projectile::copyArr(linearVelocity, {0,0,0}, speed);
        };

        // spawn the explosion
        void shoot() override{
            // create the bullet entity in the world
            Entity* explosionEntity = world->add();

            // get the json object representing bullet entity
            nlohmann::json explosionData = Projectile::spawn();

            // deserialize the entity data to render it and add bullet data
            explosionEntity->deserialize(explosionData);
            
            // get the collider component
            ColliderComponent* explosionCollider = explosionEntity->getComponent<ColliderComponent>();
            
            // set collider attributes
            explosionCollider->setEntity(explosionEntity);

            // push the entity to the colliders array
            world->addDynamicEntity(explosionEntity);

            // add the laser bullet component to the entity to use the hit function later
            explosionEntity->appendComponent<Explosion>(this);
        };

        // returns true of the hit entity's health is depleted
        bool hit(World* world, Entity* hitEntity) override
        {
            if (hitEntity->health != FLT_MAX)           // not a static object (a wall for example)
            {    
                hitEntity->health -= damage;            // decrease enemy's health
                if (hitEntity->health <= 0)             // if no remaining health remove the enemy
                {
                    world->markForRemoval(hitEntity);
                    return true;
                }
            }
            return false;
        };

        bool checkBulletRemoval()
        {
            return glfwGetTime() - spawnTime >= lifeTime;
        }
    };    
}