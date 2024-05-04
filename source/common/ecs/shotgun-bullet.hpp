#pragma once

#include "projectile.hpp"
#include <GLFW/glfw3.h>         // to get the time

namespace our{
    class ShotgunBullet : public Projectile
    {
    private:
        World* world;
        double spawnTime;                       // the time at which the bullet is shot
        double lifeTime = 0.5f;           // the duration the bullet should stay for

    public:
        // initialize the bullet attributes
        ShotgunBullet(const float (&cameraPosition)[3], const float (&cameraRotation)[3], const float (&cameraRotationProjection)[3], World* currentWorld, bool friendlyFire)
        {
            mesh = "laser";
            material = "laser";
            scale[0] = 2;
            scale[1] = 2;
            scale[2] = 0.3;
            spawnTime = glfwGetTime();

            world = currentWorld;
            speed = 4;
            radius = 0.65;
            damage = 500;
            isFriendly = friendlyFire;

            Projectile::copyArr(position, cameraPosition);
            Projectile::copyArr(rotation, cameraRotation);
            Projectile::copyArr(linearVelocity, cameraRotationProjection, speed);
        };

        // spawn the bullet
        void shoot() override{
            // create the bullet entity in the world
            Entity* shotgunBulletEntity = world->add();

            // create a json object to have the bullet data
            nlohmann::json bulletData = Projectile::spawn();

            // deserialize the entity data to render it and add bullet data
            shotgunBulletEntity->deserialize(bulletData);
            
            // get the collider component
            ColliderComponent* shotgunBulletCollider = shotgunBulletEntity->getComponent<ColliderComponent>();
            
            // set collider attributes
            shotgunBulletCollider->setEntity(shotgunBulletEntity);

            // push the entity to the colliders array
            world->addDynamicEntity(shotgunBulletEntity);

            // add the laser bullet component to the entity to use the hit function later
            shotgunBulletEntity->appendComponent<ShotgunBullet>(this);
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