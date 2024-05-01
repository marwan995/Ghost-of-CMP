#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "../ecs/component.hpp"

#include <iostream>

namespace our
{

    enum class lightingType
    {
        SPOT,
        DIRECTIONAL,
        POINT
    };

    // Abstract class for all colliders
    class LightingComponent : public Component
    {
    public:
        lightingType type;
        glm::vec3 position;
        glm::vec3 direction;


        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float cutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;

        static std::string getID() { return "Lighting"; }

        void deserialize(const nlohmann::json &data) override
        {
            if (!data.is_object())
                return;

            // get the light type
            std::string lightTypeStr = data.value("lightType", "directional");
            if (lightTypeStr == "spot")
            {
                type = lightingType::SPOT;
            }
            else if(lightTypeStr == "point")
            {
                type = lightingType::POINT;
            }else{
                type = lightingType::DIRECTIONAL;
            }

            // get the light direction
            if (data.contains("direction"))
            {
                auto directionData = data["direction"];
                direction = glm::vec3(directionData[0], directionData[1], directionData[2]);
            }

            // get the light position
            if (data.contains("position"))
            {
                auto positionData = data["position"];
                position = glm::vec3(positionData[0], positionData[1], positionData[2]);
            }

            // get the light ambient
            if (data.contains("ambient"))
            {
                auto ambientData = data["ambient"];
                ambient = glm::vec3(ambientData[0], ambientData[1], ambientData[2]);
            }

            // get the light diffuse
            if (data.contains("diffuse"))
            {
                auto diffuseData = data["diffuse"];
                diffuse = glm::vec3(diffuseData[0], diffuseData[1], diffuseData[2]);
            }

            // get the light specular
            if (data.contains("specular"))
            {
                auto specularData = data["specular"];
                specular = glm::vec3(specularData[0], specularData[1], specularData[2]);
            }

            // get light cut off
            cutOff = data.value("cutOff", 0.0f);

            // get light outer cut off
            outerCutOff = data.value("outerCutOff", 0.0f);

            // get light constant
            constant = data.value("constant", 0.0f);

            // get light linear
            linear = data.value("linear", 0.0f);
            
            // get light quadratic
            quadratic = data.value("quadratic", 0.0f);
        };
    };
}