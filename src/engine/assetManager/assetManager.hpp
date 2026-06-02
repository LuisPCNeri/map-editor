#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__

#include <SDL2/SDL.h>

#include <map>
#include <string>


namespace Managers
{
    class AssetManager {
        private:
            std::map<std::string, SDL_Texture*> gbl_assets;
        public:
            AssetManager();
            int8_t AddAsset(std::string filepath, SDL_Texture* texture);
            SDL_Texture* GetAsset(std::string filepath);
            int8_t RemoveAsset(std::string filepath);
            ~AssetManager();
            void Clear();
    };
}

extern Managers::AssetManager glblAssetManager;

#endif