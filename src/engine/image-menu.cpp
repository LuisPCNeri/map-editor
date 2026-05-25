#include "menu.hpp"
#include <cmath>
#include <iostream>

#include "engine/assetManager/assetManager.hpp"

extern Managers::AssetManager glblAssetManager;
namespace Menu {

    int8_t UsableImage::SetImage(SDL_Texture* tex) {
        this->texture = tex;
        return 0;
    }

    void UsableImage::SelectImage(Map::MapViewport vp) {
        // Logic for selecting this specific image

        SDL_Texture* texture = glblAssetManager.GetAsset(this->fpath);
        if(!texture) return;
        
        for(auto tile : vp.selected_tiles) {
            tile.second->texture = texture;
        }
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

        for (int i = 0; i < numImg; i++) {
            int grid_x = (i % cols) * this->image_size;
            int grid_y = (i / cols) * this->image_size;

            SDL_Rect dest_rect = {grid_x, grid_y, this->image_size, this->image_size};
            SDL_BlitSurface(this->raw_surfaces[i], NULL, atlas, &dest_rect);
        }

        SDL_SaveBMP(atlas, "resources/tiles.bmp");
        SDL_FreeSurface(atlas);
    }

    int8_t ImageMenu::ImportImage(const std::string& filepath, SDL_Renderer* rend) {

        /// Checks if the image exists inside the importedImages set that contains the file path of all imported images
        if(this->importedImages.find(filepath) != this->importedImages.end()) {
            /// The image exists nothing to be done
            return 1;
        };

        SDL_Surface* newSurface = SDL_LoadBMP(filepath.c_str());

        if(!newSurface) {
            std::cerr << "Failed to load image: " << SDL_GetError() << std::endl;
            return -1;
        }

        this->raw_surfaces.push_back(newSurface);
        SDL_Texture* newTexture = SDL_CreateTextureFromSurface(rend, newSurface);

        glblAssetManager.AddAsset(filepath, newTexture);

        UsableImage newUiImg;
        newUiImg.fpath = filepath;

        if( SDL_Texture* text = glblAssetManager.GetAsset(filepath) ) {
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

        this->importedImages.insert(filepath);

        return 0;
    }

}