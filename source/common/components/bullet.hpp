#pragma once
#include "mesh-renderer.hpp"


namespace our {

    // This component denotes that any renderer should draw the given mesh using the given material at the transformation of the owning entity.
    class Bullet  : public MeshRendererComponent {
    public:
        glm::vec3 position; // Position of the bullet
        glm::vec3 velocity; // Velocity of the bullet
        float speed;        // Speed of the bullet
        bool isActive; 
        int type;

        // The ID of this component type is "Mesh Renderer"
        static std::string getID() { return "bullet"; }
//        void spawnBullet(const glm::vec3& position, const glm::vec3& direction, float speed);

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json& data) override;
    };

}