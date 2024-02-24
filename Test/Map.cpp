#include "Map.h"
#include "Graphics/ResourceManager.h"

void Map::Initialize(uint32_t xMaxTiles, uint32_t yMaxTiles, uint32_t zMaxTiles) {
    xMaxTiles_ = xMaxTiles;
    yMaxTiles_ = yMaxTiles;
    zMaxTiles_ = zMaxTiles;

    tiles_.resize(xMaxTiles_ * yMaxTiles_ * zMaxTiles_, kAirTile);

}

void Map::SetupModel() {

    auto resource = ResourceManager::GetInstance();
    auto normalModel = resource->FindModel("Tile");

    tileModels_.resize(tiles_.size());

    Vector3 centerOffset = { -(float)xMaxTiles_ * 0.5f, 0.0f, -(float)zMaxTiles_ * 0.5f };

    uint32_t i = 0;
    for (uint32_t y = 0; y < yMaxTiles_; ++y) {
        for (uint32_t z = 0; z < zMaxTiles_; ++z) {
            for (uint32_t x = 0; x < xMaxTiles_; ++x) {
                if (tiles_[i] != kAirTile) {
                    tileModels_[i] = std::make_unique<ModelInstance>();
                    tileModels_[i]->SetModel(normalModel);
                    Vector3 tileOffset = { (float)x, (float)y, (float)z };
                    tileModels_[i]->SetWorldMatrix(Matrix4x4::MakeTranslation(centerOffset + tileOffset));
                    tileModels_[i]->SetReflection(false);
                    tileModels_[i]->SetCastShadow(false);
                    tileModels_[i]->SetReciveShadow(false);
                }
                ++i;
            }
        }
    }
}
