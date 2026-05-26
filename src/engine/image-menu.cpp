#include "menu.hpp"
#include <cmath>
#include <iostream>

#include "engine/assetManager/assetManager.hpp"
#include "../globalStateHandler.hpp"

#include <filesystem>

extern Managers::AssetManager glblAssetManager;
extern globalStateHandler* stateHandler;
namespace Menu {

    int8_t UsableImage::SetImage(SDL_Texture* tex) {
        this->texture = tex;
        return 0;
    }

    void UsableImage::SelectImage(Map::MapViewport* vp) {
        // Logic for selecting this specific image

        SDL_Texture* texture = glblAssetManager.GetAsset(this->fpath);
        if(!texture) return;
        
        for(auto tile : vp->selected_tiles) {
            tile.second->texture = texture;
            tile.second->DeSelect();
        }

        /// Clears selected tiles after giving them a texture
        /// Trust me this is NECESSARY
        vp->selected_tiles.clear();
    }

    ImageMenu::ImageMenu(uint16_t img_size)
        : height(20.0f), width(100.0f) {
        
        int32_t w, h;
        SDL_GetRendererOutputSize(rend, &w, &h);

        this->image_size = img_size;
        this->rect.x = 0;
        this->rect.y = h - this->height;
        this->rect.w = (int32_t) this->width;
        this->rect.h = (int32_t) this->height;
    }

    void ImageMenu::Render() {
        SDL_RenderFillRect(rend, &this->rect);

        for (auto const& pair : this->images) {
            SDL_RenderCopy(rend, pair.second.texture, NULL, &pair.second.rect);
        }
    }

    void ImageMenu::PackAndSaveSpriteSheet() {
        int16_t numImg = this->raw_surfaces.size();
        if(numImg == 0) return;

        int8_t cols = std::ceil(std::sqrt((double) numImg));
        int32_t atlasSizePixels = cols * this->image_size;

        SDL_Surface* atlas = SDL_CreateRGBSurfaceWithFormat(0, atlasSizePixels, atlasSizePixels, 32, SDL_PIXELFORMAT_RGBA32);

        for (int16_t i = 0; i < numImg; i++) {
            int8_t grid_x = (i % cols) * this->image_size;
            int8_t grid_y = (i / cols) * this->image_size;

            SDL_Rect dest_rect = {grid_x, grid_y, this->image_size, this->image_size};
            SDL_BlitSurface(this->raw_surfaces[i], NULL, atlas, &dest_rect);
        }

        if (stateHandler && !stateHandler->currentProjectPath.empty()) {
            std::string save_path = stateHandler->currentProjectPath + "/tiles.bmp";
            
            if (SDL_SaveBMP(atlas, save_path.c_str()) != 0) {
                std::cerr << "Failed to save sprite sheet: " << SDL_GetError() << std::endl;
            } else {
                std::cout << "Successfully saved atlas to: " << save_path << std::endl;
            }
        } else {
            std::cerr << "Cannot save tiles.bmp: No project is currently open!" << std::endl;
        }

        SDL_FreeSurface(atlas);
    }

    int8_t ImageMenu::ImportImage(const std::string& filepath, SDL_Renderer* rend) {

        if (!stateHandler || stateHandler->currentProjectPath.empty()) {
            std::cerr << "Error: You must create or open a project before importing images!" << std::endl;
            return -1;
        }

        std::filesystem::path original_file(filepath);
        std::string new_destination = stateHandler->currentProjectPath + "/assets/" + original_file.filename().string();
        
        try {
            std::filesystem::copy_file(filepath, new_destination, std::filesystem::copy_options::overwrite_existing);

        } catch (std::filesystem::filesystem_error& e) {

            std::cerr << "Failed to copy imported file: " << e.what() << std::endl;
            return -1;
        }

        /// Checks if the image exists inside the importedImages set that contains the file path of all imported images
        if(this->importedImages.find(new_destination) != this->importedImages.end()) {
            /// The image exists nothing to be done
            return 1;
        };

        SDL_Surface* newSurface = SDL_LoadBMP(new_destination.c_str());

        if(!newSurface) {
            std::cerr << "Failed to load image: " << SDL_GetError() << std::endl;
            return -1;
        }

        SDL_Surface* scaledSurface = SDL_CreateRGBSurfaceWithFormat(0, this->image_size, this->image_size, 32, SDL_PIXELFORMAT_RGBA32);
        SDL_Rect stretchRect = {0, 0, this->image_size, this->image_size};

        SDL_BlitScaled(newSurface, NULL, scaledSurface, &stretchRect);
        SDL_FreeSurface(newSurface);

        std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);
        SDL_SaveBMP(scaledSurface, new_destination.c_str());

        this->raw_surfaces.push_back(scaledSurface);
        SDL_Texture* newTexture = SDL_CreateTextureFromSurface(rend, scaledSurface);

        glblAssetManager.AddAsset(new_destination, newTexture);

        UsableImage newUiImg;
        newUiImg.fpath = new_destination;

        if( SDL_Texture* text = glblAssetManager.GetAsset(new_destination) ) {
            newUiImg.SetImage(text);
        }

        int32_t current_count = this->raw_surfaces.size() - 1;
        newUiImg.rect.w = this->image_size;
        newUiImg.rect.h = this->image_size;
        newUiImg.rect.x = this->rect.x + (current_count * this->image_size);
        newUiImg.rect.y = this->rect.y;

        ImageCoord coord = {(int32_t)this->raw_surfaces.size(), 0};
        this->images[coord] = newUiImg;

        PackAndSaveSpriteSheet();

        this->importedImages.insert(new_destination);

        return 0;
    }

}