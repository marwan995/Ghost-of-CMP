#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include <systems/collision.hpp>
#include <systems/enemy.hpp>
#include <systems/unlock.hpp>
#include <components/camera.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem; 
    our::EnemySystem enemySystem;
    our::CameraComponent* camera; 
    our::UnlockSystem unlockSystem;

    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        // rest map Rooms
        getApp()->currentRoam = "";
        getApp()-> lastRoam = "";
        getApp()-> alpha = 0.5f;
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());

        // get the camera that represents the player
        for (auto entity: world.getEntities())
        {
            our::CameraComponent *worldCamera = entity->getComponent<our::CameraComponent>();
            if (worldCamera)
            {
                camera = worldCamera;
                break;
            }
        }
        
        // Initialize the unlock system
        unlockSystem.enter(cameraController.getPlayerWeaponsMap(), &camera->getOwner()->localTransform.position);

        // Initialize the collision system
        // a pointer function is used to give the collision system the access to the reduceHealth function
        collisionSystem.enter(&world, &enemySystem, our::FreeCameraControllerSystem::reduceHealth);
        // Initialize the enemy system
        enemySystem.enter(&world);
            
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);

        // update player position and enemy's fire state
        cameraController.update(&world, (float)deltaTime);
        enemySystem.update(&world, (float)deltaTime);

        // update unlock system to check for player state
        unlockSystem.update();

        // check for collisions and bullet collisions
        collisionSystem.update(&world);

        // check shotguns and explosions
        collisionSystem.updateBullets(&world);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        world.deleteMarkedEntities();

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }

        // player died
        if (camera->getOwner()->health <= 0)
        {
            getApp()->changeState("gameover");
        }
        // TODO: another check here for the player win
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};