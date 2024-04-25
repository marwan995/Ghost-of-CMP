#include "world.hpp"
#include <unordered_set>
#include <iostream>

namespace our
{

    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json &data, Entity *parent)
    {
        if (!data.is_array())
            return;
        int i  = 0;
        std::vector<nlohmann::json> obstacles;
        for (const auto &entityData : data)
        {
            // TODO: (Req 8) Create an entity, make its parent "parent" and call its deserialize with "entityData".
            Entity *entity = add();
            entity->parent = parent;
            entity->deserialize(entityData);
            if( i > 3 )
                obstacles.push_back(entityData);
            if (entityData.contains("children"))
            {
                // TODO: (Req 8) Recursively call this world's "deserialize" using the children data
                //  and the current entity as the parent
                deserialize(entityData["children"], entity);
            }
            i++;
        }
        // This part is for repeating the obstacles
        std::vector<nlohmann::json> repeatedObstacles;
        for(auto &obstacle : obstacles)
        {
            if(obstacle.contains("position")) {
                auto positionArray = obstacle["position"];
                glm::vec3 position(positionArray[0].get<float>(), positionArray[1].get<float>(), positionArray[2].get<float>());
                int repeatCount = rand() % 100 + 1;
                for(int i = 0; i < repeatCount; i++) {
                    obstacle["position"] = { position.x + rand() % 101 - 55
                                            , position.y + rand() % 6,
                                            position.z + rand() % 81 - 40};
                    repeatedObstacles.push_back(obstacle);
                }
            }
        }
        
        for(const auto &obstacle : repeatedObstacles) {
            Entity *entity = add();
            entity->parent = parent;
            entity->deserialize(obstacle);
        }
        
    }

}