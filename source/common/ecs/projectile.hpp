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
        float position[3] = {0,0,0};
        float rotation[3] = {0,0,0};
        float linearVelocity[3] = {0,0,0};

        static std::string getID() { return "Projectile"; }

        // utility to copy values from another array
        void copyArr(float (&dist)[3], const float (&src)[3], const float multiplier = 1) {
           for (int i = 0; i < 3; i++) {
            dist[i] = src[i] * multiplier;
            }
        }

        // create a json object to have the bullet entity data
        nlohmann::json spawn()
        {
            return {
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
        }

        void decreaseHealthbar(Entity* hitEntity, int damage, float maxHealth = 500, float xScale = 0.6)
        {
            auto enemy = hitEntity->getComponent<EnemyComponent>();
            if(enemy)
            {
                if(enemy->getOwner()->children.size() == 0) 
                    return;
                auto healthBar = enemy->getOwner()->children[0];
                // We should take the health of the enemy and the scale of the health bar
                float decreasedBy = (damage * xScale) / (maxHealth);
                healthBar->localTransform.scale[0] -= decreasedBy;
                healthBar->localTransform.scale[0] = std::max(0.0f, healthBar->localTransform.scale[0]);
                healthBar->localTransform.position[0] += ((decreasedBy / 2.0) * 9.6);
            }
        }

        // only overriden to be able to make projectile a component
        // can be changed later if the structure of bullet/grenade creation is changed (probably not)
        void deserialize(const nlohmann::json& data) override{};

        // function to spawn a bullet
        virtual void shoot()=0;

        // function to remove the bullet when it hits another collider
        virtual bool hit(World* world, Entity* hitEntity) = 0;

        virtual ~Projectile(){};
    };
    
}