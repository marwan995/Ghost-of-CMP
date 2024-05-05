#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "../ecs/component.hpp"

#include <iostream>

namespace our{

    enum class ColliderShape {
        RECT,
        SPHERE
    };
    enum class ColliderType {
        STATIC,
        DYNAMIC,
        BULLET
    };

// Abstract class for all colliders
class ColliderComponent : public Component{
    public:
        ColliderShape shape;
        ColliderType type;
        Entity* colliderEntity;
        float x,y,z;
        float scaleX=0, scaleY=0, scaleZ=0;
        glm::vec3 shifted = glm::vec3(0,0,0);
        float radius;

        void setEntity(Entity* entity){
            colliderEntity = entity;
            if (getOwner()->localTransform.rotation.y != 0)
            {
                x = entity->localTransform.position[0]+shifted[2];
                z = entity->localTransform.position[2]-shifted[0];
            }
            else{
                x = entity->localTransform.position[0]+shifted[0];
                z = entity->localTransform.position[2]+shifted[2];
            }
            y = entity->localTransform.position[1]+shifted[1];


        }

        static std::string getID() { return "Collider"; }

        // TODO: check if the collision depth is needed
        static bool isColliding(glm::vec3 vector){
            return vector != glm::vec3(0,0,0);
        }
        
       void deserialize(const nlohmann::json& data) override
       {
            if (!data.is_object())
                return;

            // get the collider shape
            std::string colliderShapeStr = data.value("colliderShape", "rect");
            if(colliderShapeStr == "sphere"){
                shape = ColliderShape::SPHERE;
            }else{
                shape = ColliderShape::RECT;
            }

            // get the collider type
            std::string colliderTypeStr = data.value("colliderType", "static");
            if(colliderTypeStr == "dynamic"){
                type = ColliderType::DYNAMIC;
            }else if (colliderTypeStr == "bullet")
            {
                type = ColliderType::BULLET;
            }
            else{
                type = ColliderType::STATIC;
            }

            // get the collider scale (in case it's rect)
            if (data.contains("scale"))
            {
                auto scale = data["scale"];
                scaleX = scale[0];
                scaleY = scale[1];
                scaleZ = scale[2];
            }
            if (data.contains("shifted"))
            {
                auto shiftAmount = data["shifted"];
                shifted[0] = shiftAmount[0];
                shifted[1] = shiftAmount[1];
                shifted[2] = shiftAmount[2];
            }
            
            // get collider radius (in case it's sphere)
            radius = data.value("radius", 0.0f);

            if (getOwner()->localTransform.rotation.y != 0)
            {
                std::swap(scaleX, scaleZ);
            }
        };



        glm::vec3 collisionDepth(ColliderComponent* other)
        {

            x = colliderEntity->localTransform.position[0]+shifted[0];
            y = colliderEntity->localTransform.position[1]+shifted[1];
            z = colliderEntity->localTransform.position[2]+shifted[2];

            if (shape == ColliderShape::SPHERE && other->shape == ColliderShape::SPHERE)
            {
                return collisionDepthBetweenSphereAndSphere(other);
            }
            else if (shape == ColliderShape::SPHERE && other->shape == ColliderShape::RECT)
            {
                return collisionDepthBetweenSphereAndRect(other);
            }
            else if (shape == ColliderShape::RECT && other->shape == ColliderShape::RECT)
            {
                return collisionDepthRectAndRect(other);
            }
            else if (shape == ColliderShape::RECT && other->shape == ColliderShape::SPHERE)
            {
                return collisionDepthBetweenRectAndSphere(other);
            }
            return {0,0,0};
        };

        glm::vec3 collisionDepthBetweenSphereAndSphere(ColliderComponent* other)
        {
            if ( glm::distance(glm::vec3(x,y,z), glm::vec3(other->x,other->y,other->z)) < radius + other->radius )
            {
                return {
                    other->x - x,
                    other->y - y,
                    other->z - z
                };
            }
            return {0,0,0};
        }

        glm::vec3 collisionDepthBetweenSphereAndRect(ColliderComponent* other){
            float closestX = glm::clamp(x, other->x - other->scaleX / 2.0f, other->x + other->scaleX / 2.0f);
            float closestY = glm::clamp(y, other->y - other->scaleY / 2.0f, other->y + other->scaleY / 2.0f);
            float closestZ = glm::clamp(z, other->z - other->scaleZ / 2.0f, other->z + other->scaleZ / 2.0f);

            // Calculate the distance between the closest point and the sphere center
            glm::vec3 closestPoint(closestX, closestY, closestZ);
            glm::vec3 sphereCenter(x, y, z);
            glm::vec3 direction = sphereCenter - closestPoint;
            float distance = glm::length(direction);

            // If the distance is less than or equal to the sphere radius, they are colliding
            if (distance <= radius) {
                // Calculate the penetration depth along each axis
                glm::vec3 penetrationDepth = direction * (radius - distance) / distance;
                return penetrationDepth;
            }

            // No collision, return zero vector
            return glm::vec3(0.0f);
        }

        glm::vec3 collisionDepthRectAndRect(ColliderComponent* other){
            if((x < other->x + other->scaleX) && (x + scaleX > other->x) && (y < other->y + other->scaleY) && (y + scaleY > other->y) && (z < other->z + other->scaleZ) && (z + scaleZ > other->z)){
                return {
                    other->x - x,
                    other->y - y,
                    other->z - z,
                };
            }
            return {0,0,0};
        }
        
        glm::vec3 collisionDepthBetweenRectAndSphere(ColliderComponent* other){
            float x = glm::clamp(other->x, this->x, this->x + scaleX);
            float y = glm::clamp(other->y, this->y, this->y + scaleY);
            float z = glm::clamp(other->z, this->z, this->z + scaleZ);
            float distance = glm::distance(glm::vec3(x,y,z), glm::vec3(other->x, other->y, other->z));
            if (distance < other->radius)
            {
                return {
                    other->x - x,
                    other->y - y,
                    other->z - z
                };
            }
            return {0,0,0};
        }
};
}