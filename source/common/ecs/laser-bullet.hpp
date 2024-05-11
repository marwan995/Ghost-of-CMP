#pragma once

#include "projectile.hpp"

namespace our{
    class LaserBullet : public Projectile
    {
    private:
        World* world;

    public:
        // initialize the bullet attributes
        LaserBullet(const float (&cameraPosition)[3], const float (&cameraRotation)[3], const float (&cameraRotationProjection)[3], World* currentWorld, bool friendlyFire)
        {
            mesh = "laser";
            material = "laser";
            scale[0] = 0.2;
            scale[1] = 0.2;
            scale[2] = 0.01;
            
            world = currentWorld;
            speed = 10;
            radius = 0.2;
            damage = 30;
            isFriendly = friendlyFire;

            Projectile::copyArr(position, cameraPosition);
            Projectile::copyArr(rotation, cameraRotation);
            Projectile::copyArr(linearVelocity, cameraRotationProjection, speed);
        };

        // spawn the bullet
        void shoot() override{
            // create the bullet entity in the world
            Entity* laserBulletEntity = world->add();

            // get the json object representing bullet entity
            nlohmann::json bulletData = Projectile::spawn();

            // deserialize the entity data to render it and add bullet data
            laserBulletEntity->deserialize(bulletData);
            
            // get the collider component
            ColliderComponent* laserBulletCollider = laserBulletEntity->getComponent<ColliderComponent>();
            
            // set collider attributes
            laserBulletCollider->setEntity(laserBulletEntity);

            // push the entity to the colliders array
            world->addDynamicEntity(laserBulletEntity);

            // add the laser bullet component to the entity to use the hit function later
            laserBulletEntity->appendComponent<LaserBullet>(this);
        };

        // returns true of the hit entity's health is depleted
        bool hit(World* world, Entity* hitEntity) override
        {
            if (hitEntity->health != FLT_MAX)           // not a static object (a wall for example)
            {    
                hitEntity->health -= damage;            // decrease 's health
                decreaseHealthbar(hitEntity, damage, hitEntity->maxHealth, hitEntity->healthBarScale);
                if (hitEntity->health <= 0)             // if no remaining health remove the 
                {
                    world->markForRemoval(hitEntity);
                    return true;
                }
            }
            return false;
        };
    };    
}