#pragma once

#include "entity.hpp"
// #include "world"
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../components/collision-component.hpp"
#include "../systems/collision.hpp"
#include "component.hpp"

#include <iostream>

namespace our {
    
    // enum class ProjectileType {
    //     LASER,
    //     GRENADE,
    //     ROCKET
    // };
    
    // TODO: make projectile a component and resolve the connection with the collision
    class Projectile : public Component
    {   
    public:
        // speed of the bullet
        float speed;
        // radius of the bullet effect
        float radius;
        // to control the rate of fire
        // float bulletsPerSecond;
        // the amount of damage the bullet inflects
        float damage;
        // type of the gun
        // ProjectileType type;
        bool isFriendly;

        // projectile entity attributes
        std::string mesh;
        std::string material;
        float scale[3];
        float position[3];
        float rotation[3];
        float linearVelocity[3];

        static std::string getID() { return "Projectile"; }

        // utility to copy values from another array
        void copyArr(float (&dist)[3], const float (&src)[3], const float multiplier = 1) {
           for (int i = 0; i < 3; i++) {
            dist[i] = src[i] * multiplier;
            }
        }

        // only overriden to be able to make projectile a component
        // can be changed later if the structure of bullet/grenade creation is changed (probably not)
        void deserialize(const nlohmann::json& data) override{};

        // function to spawn a bullet
        virtual void shoot()=0;

        // function to remove the bullet when it hits another collider
        virtual bool hit(World* world,Entity* projectile,Entity* hitEntity) = 0;
    };
    
}