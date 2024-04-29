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
                            {"type", "Mesh Renderer"},
                            {"mesh", mesh},
                            {"material", material}
                        },
                        {
                            {"type", "Movement"},
                            {"linearVelocity", linearVelocity} 
                        }
                    })}
                };

            // set collider component
            laserBulletEntity->addComponent<ColliderComponent>();
            // get the collider component
            ColliderComponent* laserBulletCollider = laserBulletEntity->getComponent<ColliderComponent>();
            
            // set collider attributes
            laserBulletCollider->setEntity(laserBulletEntity);
            laserBulletCollider->shape = ColliderShape::SPHERE;
            laserBulletCollider->type = ColliderType::BULLET;
            laserBulletCollider->radius =radius;

            // push the entity to the colliders array
            CollisionSystem::addDynamicEntity(laserBulletEntity);

            // deserialize the entity data to render it
            laserBulletEntity->deserialize(bulletData);
        };

        void hit() override
        {

        };
    };    
}