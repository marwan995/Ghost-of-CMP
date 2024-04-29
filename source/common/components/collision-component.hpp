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
        float scaleX, scaleY, scaleZ;
        glm::vec3 shifted = glm::vec3(0,0,0);
        float radius;

        void setEntity(Entity* entity){
            colliderEntity = entity;
            x = entity->localTransform.position[0]+shifted[0];
            y = entity->localTransform.position[1]+shifted[1];
            z = entity->localTransform.position[2]+shifted[2];
        }

        static std::string getID() { return "Collider"; }

        // TODO: check if the collision depth is needed
        static bool isColliding(glm::vec3 vector){
            return vector != glm::vec3(0,0,0);
        }
        
       void deserialize(const nlohmann::json& data) override
       {
            std::cout<<"collider\n";
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
            }else{
                type = ColliderType::STATIC;
            }

            // if (data.contains("position"))
            // {
            //     auto position = data["position"];
            //     x = position[0];
            //     y = position[1];
            //     z = position[2];
            // }
            // x = getOwner()->localTransform.position[0];
            // y = getOwner()->localTransform.position[1];
            // z = getOwner()->localTransform.position[2];

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
        };



        glm::vec3 collisionDepth(ColliderComponent* other)
        {
            x = colliderEntity->localTransform.position[0]+shifted[0];
            y = colliderEntity->localTransform.position[1]+shifted[1];
            z = colliderEntity->localTransform.position[2]+shifted[2];

            // std::cout<<glm::distance(glm::vec3(x,y,z), other->colliderEntity->localTransform.position)<<std::endl;

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
            float x = glm::clamp(this->x, other->x, other->x + other->scaleX);
            float y = glm::clamp(this->y, other->y, other->y + other->scaleY);
            float z = glm::clamp(this->z, other->z, other->z + other->scaleZ);
            float distance = glm::distance(glm::vec3(x,y,z), glm::vec3(this->x, this->y, this->z));
            if (distance < radius)
            {
                return {
                    this->x - x,
                    this->y - y,
                    this->z - z
                };
            }
            return {0,0,0};
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