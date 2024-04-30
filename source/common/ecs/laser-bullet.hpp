#pragma once

#include "projectile.hpp"

namespace our{
    class LaserBullet : public Projectile
    {
    private:
        std::string mesh = "laser";
        std::string material = "laser";
        float scale[3] = {0.2, 0.2, 0.01};
        float position[3];
        float rotation[3];
        float linearVelocity[3];
        World* world;

    public:
        LaserBullet() = default;

        // initialize the bullet attributes
        LaserBullet(const float (&cameraPosition)[3], const float (&cameraRotation)[3], const float (&cameraRotationProjection)[3], World* currentWorld)
        {
            world = currentWorld;
            speed = 10;
            radius = 0.2;
            damage = 30;

            Projectile::copyArr(position, cameraPosition);
            Projectile::copyArr(rotation, cameraRotation);
            Projectile::copyArr(linearVelocity, cameraRotationProjection, speed);
        };

        // spawn the bullet
        void shoot() override{
            // create the bullet entity in the world
            Entity* laserBulletEntity = world->add();

            // create a json object to have the bullet data
            nlohmann::json bulletData = {
                    {"position", position},
                    {"rotation", rotation},
                    {"scale", scale},
                    {"components", nlohmann::json::array({
                        {
                            // bullet model
                            {"type", "Mesh Renderer"},
                            {"mesh", mesh},
                            {"material", material}
                        },
                        {
                            // bullet motion
                            {"type", "Movement"},
                            {"linearVelocity", linearVelocity} 
                        },
                        {
                            // collider attributes
                            {"type", "Collider"},
                            {"colliderShape", "sphere"},
                            {"colliderType", "bullet"},
                            {"radius", radius}
                        }
                    })}
                };

            // deserialize the entity data to render it and add bullet data
            laserBulletEntity->deserialize(bulletData);
            
            // get the collider component
            ColliderComponent* laserBulletCollider = laserBulletEntity->getComponent<ColliderComponent>();
            
            // set collider attributes
            laserBulletCollider->setEntity(laserBulletEntity);

            // push the entity to the colliders array
            CollisionSystem::addDynamicEntity(laserBulletEntity);
            // addProjectile2Colliders(laserBulletEntity);

            // laserBulletEntity->addComponent<LaserBullet>();
        };

        void hit() override
        {
            std::cout<<"hit\n";
        };
    };    
}