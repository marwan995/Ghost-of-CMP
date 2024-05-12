#pragma once

#include "../ecs/world.hpp"
#include "../components/enemy-component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "../components/camera.hpp"

// bullet types
#include "../ecs/laser-bullet.hpp"
#include "../ecs/shotgun-bullet.hpp"
#include "../ecs/rocket-bullet.hpp"
#include "../ecs/explosion.hpp"

namespace our
{
    // The Enemy system is responsible for attacking the player.
    class EnemySystem
    {
        CameraComponent *camera;
        std::vector<Entity *> *enemiesEntities;
        float previousTime = 0;
    public:
        void enter(World *world)
        {
            // get access to the needed entities
            enemiesEntities = &world->enemiesEntities;

            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                EnemyComponent *enemy = entity->getComponent<EnemyComponent>();

                CameraComponent *worldCamera = entity->getComponent<CameraComponent>();
                if (worldCamera)
                {
                    camera = worldCamera;
                }

                // If the movement component exists
                if (enemy)
                {
                    enemiesEntities->push_back(entity);
                }
            }
        }

        void update(World *world, double deltaTime)
        {
            // get the camera collider
            ColliderComponent *cameraCollider = camera->getOwner()->getComponent<ColliderComponent>();
            // For each entity in the world
            for (auto entity : *enemiesEntities)
            {
                // Get the movement component if it exists
                EnemyComponent *enemy = entity->getComponent<EnemyComponent>();

                // If the movement component exists
                if (enemy)
                {
                    ColliderComponent *enemyCollider = entity->getComponents<ColliderComponent>()[1];
                    if (!enemyCollider)
                    {
                        break;
                    }
                    glm::vec3 collisionDepth = enemyCollider->collisionDepth(cameraCollider);

                    if (ColliderComponent::isColliding(collisionDepth))
                    {
                        
                        if (enemy->type == EnemyType::MELEE)
                        {
                            enemy->aimAt(camera,true);
                            enemy->moveTowardsTarget(camera->getOwner(),deltaTime,deltaTime);
                        }
                        else if (enemy->type ==EnemyType::BOSS1){

                            enemy->aimAt(camera);
                            if (enemy->checkRateOfFire()&&!enemy->boss1_following)
                            {
                                glm::vec3 rotation = enemy->getOwner()->localTransform.rotation;
                                glm::vec3 position = enemy->getOwner()->localTransform.position;
                                glm::vec3 direction = -glm::normalize(position - camera->getOwner()->localTransform.position);
                                float bulletRotation[3] = { glm::degrees(rotation.x), glm::degrees(rotation.y), glm::degrees(rotation.z)};
                                float bulletMovementDirections[3] = {direction[0],direction[1],direction[2]};
                                float bulletPosition[3] = {position.x + direction[0] * 2, position.y + direction[1] * 2, position.z + direction[2] * 2};

                                RocketBullet* rocketBullet = new RocketBullet(bulletPosition, bulletRotation,bulletMovementDirections , world, false,true);
                                rocketBullet->shoot();
                            }
                            float distance = glm::length(enemy->getOwner()->localTransform.position - camera->getOwner()->localTransform.position);
                            if(distance < 7){
                                enemy->moveTowardsTarget(camera->getOwner(),deltaTime,deltaTime,true);
                                enemy->boss1_following = true;
                            }
                            else if(glm::length(enemy->getOwner()->localTransform.position - enemy->boss1_home)>1){
                                enemy->boss1_back(deltaTime);
                                enemy->boss1_following = false;
                            }                         

                        }
                        else if (enemy->type ==EnemyType::BOSS2)
                        {
                            enemy->aimAt(camera);
                            if (enemy->checkRateOfFire())
                            {
                                glm::vec3 rotation = enemy->getOwner()->localTransform.rotation;
                                glm::vec3 position = enemy->getOwner()->localTransform.position;
                                glm::vec3 direction = -glm::normalize(position - camera->getOwner()->localTransform.position);
                                float bulletRotation[3] = { glm::degrees(rotation.x), glm::degrees(rotation.y), glm::degrees(rotation.z)};
                                float bulletMovementDirections[3] = {direction[0],direction[1],direction[2]};
                                float bulletPosition[3] = {position.x + direction[0] * 2, position.y + direction[1] * 2, position.z + direction[2] * 2};

                                LaserBullet* laserBullet = new LaserBullet(bulletPosition, bulletRotation,bulletMovementDirections , world, false);
                                laserBullet->shoot();
                            }

                            float distance =glm::length(enemy->getOwner()->localTransform.position - camera->getOwner()->localTransform.position);
                            if(distance < 15){
                                enemy->moveTowardsTarget(camera->getOwner(),deltaTime,deltaTime);
                                float curTime = glfwGetTime();
                                if(curTime - previousTime >= 8) // wait 7 seconds before rendering some spiders
                                { 
                                    previousTime = curTime;
                                    addBossSpiders(world);
                                }
                            }
                        }
                        else if (enemy->type == EnemyType::SHOOTER)
                        {
                            enemy->aimAt(camera);
                            if (enemy->checkRateOfFire())
                            {
                                glm::vec3 rotation = enemy->getOwner()->localTransform.rotation;
                                glm::vec3 position = enemy->getOwner()->localTransform.position;
                                glm::vec3 direction = -glm::normalize(position - camera->getOwner()->localTransform.position);
                                float bulletRotation[3] = { glm::degrees(rotation.x), glm::degrees(rotation.y), glm::degrees(rotation.z)};

                                float bulletMovementDirections[3] = {direction[0],direction[1],direction[2]};
                                float bulletPosition[3] = {position.x + direction[0] * 2, position.y + direction[1] * 2, position.z + direction[2] * 2};

                                LaserBullet *laserBullet = new LaserBullet(bulletPosition, bulletRotation, bulletMovementDirections, world, false);
                                laserBullet->shoot();
                            }
                        }
                    }
                }
            }
        }

        void addBossSpiders(World *world)
        {
            // Create a JSON object for the spider entity
            std::filesystem::path currentPath = std::filesystem::current_path();
            currentPath.append("config");
            currentPath.append("boss-spiders.jsonc");
            std::ifstream inputFile(currentPath);
            std::vector<nlohmann::json> spidersData;
            if (inputFile)
            {
                nlohmann::json jsonData = nlohmann::json::parse(inputFile, nullptr, true, true);
                if (jsonData.contains("nodes"))
                {
                    auto nodes = jsonData["nodes"];
                    for(auto &node : nodes) {
                        spidersData.push_back(node);
                    }
                }
                inputFile.close();
            }else{
                std::cerr << "Error opening file: " << strerror(errno) << std::endl;
            }
            
            for(auto &spiderData : spidersData)
            {
                Entity *spider = world->add();
                spider->deserialize(spiderData);
                auto colliders = spider->getComponents<ColliderComponent>();
                ColliderComponent *staticCollider;
                ColliderComponent *dynamicCollider;
                for(auto collider : colliders)
                {
                    if(collider->type == ColliderType::STATIC)
                    {
                        staticCollider = collider;
                    }
                    else if(collider->type == ColliderType::DYNAMIC)
                    {
                        dynamicCollider = collider;
                    }
                }

                staticCollider->setEntity(spider);
                dynamicCollider->setEntity(spider);
                world->addDynamicEntity(spider);
                world->staticEntities.push_back(spider);
                world->enemiesEntities.push_back(spider);
            }
        }

        void enemyKilled(Entity *killedEntity)
        {
            auto it = std::find(enemiesEntities->begin(), enemiesEntities->end(), killedEntity);
            enemiesEntities->erase(it);
        }
    };
}