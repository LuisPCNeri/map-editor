#include "assetManager.hpp"

#include <iostream>

namespace Managers {

    AssetManager::AssetManager() {}

    AssetManager::~AssetManager() {
        for(auto const& pair : this->gbl_assets) {
            if(pair.second != nullptr) SDL_DestroyTexture(pair.second);
        }

        this->gbl_assets.clear();
    }

    int8_t AssetManager::AddAsset(std::string filepath, SDL_Texture* texture) {
        this->gbl_assets.insert({filepath, texture});
        return 0;
    }

    SDL_Texture* AssetManager::GetAsset(std::string filepath) {
        auto it = this->gbl_assets.find(filepath);
        
        /// Texture not found in the glbl manager
        if(it == this->gbl_assets.end()) {
            std::cerr << "Error: Could not find the image in the global asset manager -> " << filepath << std::endl;
            return nullptr; 
        }

        return it->second;
    }

    int8_t AssetManager::RemoveAsset(std::string filepath) {
        auto it = this->gbl_assets.find(filepath);

        if(it != this->gbl_assets.end()) {

            SDL_DestroyTexture(it->second);
            this->gbl_assets.erase(it);
            return 0;
        }

        return -1;
    }

}