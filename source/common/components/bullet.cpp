#include "../asset-loader.hpp"
#include "bullet.hpp"
namespace our
{
    // Receives the mesh & material from the AssetLoader by the names given in the json object
    void Bullet::deserialize(const nlohmann::json &data)
    {
        MeshRendererComponent::deserialize(data);
    }
}