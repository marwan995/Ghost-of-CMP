#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include "collision.hpp"
#include "../components/collision-component.hpp"

#include "../ecs/laser-bullet.hpp"

#include <iostream>

namespace our
{
    // WARNING: this class is used as a player class as it's a FPS game

    // Global constant
    const float avgDeltaTime = 0.008335638028169f;

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem
    {
        Application *app; // The application in which the state runs

        int activeWeapon = 0;
        int deltasCounter = 0;
        const float weapons_BPS[3] = {10, 1, 1}; // holds weapons bullets per seconds

        // utility to return true if a bullet should be spawned
        bool checkRateOfFire()
        {
            // get current weapon BPS
            float bulletsPerSecond = weapons_BPS[activeWeapon];
            // check for it's cooldown
            if (deltasCounter == 0)
            {
                deltasCounter++;
                return true;
            }
            else if ((deltasCounter)*avgDeltaTime >= (1 / bulletsPerSecond))
            {
                deltasCounter = 0;
            }
            else
            {
                deltasCounter++;
            }
            return false;
        }

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
            app->getMouse().lockMouse(app->getWindow()); // lock the mouse when play state is entered
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void update(World *world, float deltaTime)
        {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent *camera = nullptr;
            FreeCameraControllerComponent *controller = nullptr;

            for (auto entity : world->getEntities())
            {
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if (camera && controller)
                    break;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if (!(camera && controller))
                return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity *entity = camera->getOwner();

            // We get a reference to the entity's position and rotation
            glm::vec3 &position = entity->localTransform.position;
            glm::vec3 &rotation = entity->localTransform.rotation;
            camera->lastPosition = entity->localTransform.position;

            // Mouse left click (shoot fire)
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1))
            {
                if (checkRateOfFire()) // if it's time to spawn a bullet or not
                {
                    // calculate bullet direction & speed in all 3 directions
                    float bulletRotation[3] = {180 - glm::degrees(rotation.x), glm::degrees(rotation.y) - 180, glm::degrees(rotation.z)};
                    float bulletSpeedX = -cos(-rotation.x) * sin(rotation.y);
                    float bulletSpeedY = -sin(-rotation.x);
                    float bulletSpeedZ = -cos(-rotation.x) * cos(rotation.y);
                    float bulletMovementDirections[3] = {bulletSpeedX, bulletSpeedY, bulletSpeedZ};
                    float bulletPosition[3] = {position.x + bulletSpeedX / 4, position.y + bulletSpeedY / 4, position.z + bulletSpeedZ / 4};
                    if (activeWeapon == 0)
                    {
                        // laser rifle
                        LaserBullet *laserBullet = new LaserBullet(bulletPosition, bulletRotation, bulletMovementDirections, world);
                        laserBullet->isFriendly = true;
                        laserBullet->shoot();
                        world->audioPlayer.play("Laser.wav");
                    }
                    else if (activeWeapon == 1)
                    {
                        std::cout << "weapon2\n";
                    }
                    else if (activeWeapon == 2)
                    {
                        std::cout << "weapon3\n";
                    }
                }
            }

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation

            // Mouse movement is always checked
            glm::vec2 delta = app->getMouse().getMouseDelta();
            rotation.x -= delta.y * controller->rotationSensitivity; // The y-axis controls the pitch
            rotation.y -= delta.x * controller->rotationSensitivity; // The x-axis controls the yaw

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if (rotation.x < -glm::half_pi<float>() * 0.99f)
                rotation.x = -glm::half_pi<float>() * 0.99f;
            if (rotation.x > glm::half_pi<float>() * 0.99f)
                rotation.x = glm::half_pi<float>() * 0.99f;

            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time.
            rotation.y = glm::wrapAngle(rotation.y);
            position.y = glm::clamp(position.y, 0.0f, controller->max_y);

            // We update the camera fov based on the mouse wheel scrolling amount
            // TODO: change fov on right mouse click
            // float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            // fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI

            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_2))
            {
                camera->fovY = glm::pi<float>() * 0.1f;
                entity->children[entity->children.size() - 1]->localTransform.scale = glm::vec3(0.07, 0.07, 0.07);
                entity->children[0]->localTransform.position = glm::vec3(0, -1.05, -1.1);
                entity->children[0]->localTransform.rotation = glm::vec3(0, 0, 0);
            }
            else
            {
                camera->fovY = .49 * glm::pi<float>();
                entity->children[entity->children.size() - 1]->localTransform.scale = glm::vec3(0.0088, 0.0088, 0.0088);
                entity->children[0]->localTransform.position = glm::vec3(1, -1, -1);
                entity->children[0]->localTransform.rotation = glm::radians(glm::vec3(0, 30, 0));

            } // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)),
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;

            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
                current_sensitivity *= controller->speedupFactor;

            // We change the camera position based on the keys WASD
            // S & W moves the player back and forth
            // forward/backward motion
            // Project y-component to x-z plane
            float yProjection = front.y * sin(rotation.x);
            // Ignore the y-component of front and project yProjection to x and z axes
            glm::vec3 forwardMotion = glm::vec3(front.x - yProjection * sin(rotation.y), 0.0f, front.z - yProjection * cos(rotation.y));

            if (app->getKeyboard().isPressed(GLFW_KEY_W))
            {
                position += forwardMotion * (deltaTime * current_sensitivity.z);
            }
            if (app->getKeyboard().isPressed(GLFW_KEY_S))
                position -= forwardMotion * (deltaTime * current_sensitivity.z);

            // A & D moves the player left or right
            if (app->getKeyboard().isPressed(GLFW_KEY_D))
                position += right * (deltaTime * current_sensitivity.x);
            if (app->getKeyboard().isPressed(GLFW_KEY_A))
                position -= right * (deltaTime * current_sensitivity.x);

            // Check if jump key is pressed
            if (app->getKeyboard().isPressed(GLFW_KEY_SPACE) || app->getKeyboard().isPressed(GLFW_KEY_LEFT_ALT))
            {
                if (!controller->isJumping)
                {
                    controller->verticalVelocity = 8.0f; // Initial jump velocity
                    controller->isJumping = true;
                }
            }

            // Apply gravity
            controller->verticalVelocity -= controller->gravity * deltaTime;
            position.y += controller->verticalVelocity * deltaTime;

            // Check if camera is on the ground level
            if (position.y <= 0.0f)
            {
                position.y = 0.0f;                   // Clamp camera position to ground level
                controller->verticalVelocity = 0.0f; // Reset vertical velocity
                controller->isJumping = false;       // Reset jump flag
            }

            // Check for weapon change
            if (app->getKeyboard().isPressed(GLFW_KEY_1) && activeWeapon != 0)
            {
                activeWeapon = 0;
                deltasCounter = 0;
            }
            else if (app->getKeyboard().isPressed(GLFW_KEY_2) && activeWeapon != 1)
            {
                activeWeapon = 1;
                deltasCounter = 0;
            }
            else if (app->getKeyboard().isPressed(GLFW_KEY_3) && activeWeapon != 2)
            {
                activeWeapon = 2;
                deltasCounter = 0;
            }
            locationInMap(camera);
        }
        void locationInMap(CameraComponent *camera)
        {
            auto position = camera->getOwner()->localTransform.position;
            if(position[1] == 0.0 &&  (position[0] > -8.5  && position[0] < 7.5) &&(position[2] > 1  && position[1] < 12.45)&&app->alpha==1.0f)//&&app->alpha<=0)
                { 
                    app->currentRoam = "GPU BOOSTER";
                    std::cout<<  app->currentRoam<<"\n";
                    
                } 
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit()
        {
            app->getMouse().unlockMouse(app->getWindow());
        }
    };
}
