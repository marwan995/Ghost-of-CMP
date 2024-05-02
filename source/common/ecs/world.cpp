#include "world.hpp"
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <filesystem>


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
                if(obstacle.contains("objectType")){
                    std::string objectType = obstacle["objectType"];
                    if(objectType == "wall")
                        deserializeObjects(obstacle, repeatedObstacles, "wall.jsonc");
                    else if(objectType == "cube"){
                        deserializeObjects(obstacle, repeatedObstacles, "cube.jsonc");
                    }
                }
            }
        }
        
        for(const auto &obstacle : repeatedObstacles) {
            Entity *entity = add();
            entity->parent = parent;
            entity->deserialize(obstacle);
        }
        
    }
    void World::deserializeObjects(nlohmann::json &obstacle, std::vector<nlohmann::json>& repeatedObstacles, std::string fileName) {
        //TODO: this should be changed to the path of the json file containing the wall
        // print the current directory path
        std::filesystem::path currentPath = std::filesystem::current_path();
        currentPath.append("config");
        currentPath.append(fileName);
        std::ifstream inputFile(currentPath);
        if (inputFile)
        {
            nlohmann::json jsonData = nlohmann::json::parse(inputFile, nullptr, true, true);

            if (jsonData.contains("nodes"))
            {
                auto nodes = jsonData["nodes"];
                for(auto &node : nodes) {
                    nlohmann::json newObstacle = obstacle;
                    newObstacle["position"] = node["position"];
                    newObstacle["rotation"] = node["rotation"];
                    repeatedObstacles.push_back(newObstacle);
                }
            }
            inputFile.close();
        }else{
            std::cerr << "Error opening file: " << strerror(errno) << std::endl;
        }
    }
}