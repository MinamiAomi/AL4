#pragma once
#include <vector>
#include <list>
#include <memory>

#include "Graphics/Model.h"


class Map {
public:
    enum TileType {
        kAirTile,       //     空気
        kNormalTile,    // 普通
    
    };

    void Initialize(uint32_t xMaxTiles, uint32_t yMaxTiles, uint32_t zMaxTiles);
    void SetupModel();

  

    TileType& GetTile(uint32_t x, uint32_t y, uint32_t z) { return tiles_.at(x + z * xMaxTiles_ + y * xMaxTiles_ * zMaxTiles_); }
    
    uint32_t TileArrayIndex(uint32_t x, uint32_t y, uint32_t z) const { return x + z * xMaxTiles_ + y * xMaxTiles_ * zMaxTiles_; }
    void TileSpaceIndex(uint32_t arrayIndex, uint32_t& x, uint32_t& y, uint32_t& z) {
        y = arrayIndex / (xMaxTiles_ * zMaxTiles_);
        z = (arrayIndex - (y * xMaxTiles_ * zMaxTiles_)) / xMaxTiles_;
        x = (arrayIndex - (z * xMaxTiles_ + y * xMaxTiles_ * zMaxTiles_));
    }

private:

    std::vector<std::unique_ptr<ModelInstance>> tileModels_;
    std::vector<TileType> tiles_;


    // マップの最大数
    uint32_t xMaxTiles_ = 0;
    uint32_t yMaxTiles_ = 0;
    uint32_t zMaxTiles_ = 0;
};
