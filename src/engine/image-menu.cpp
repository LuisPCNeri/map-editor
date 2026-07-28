#include "menu.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "engine/assetManager/assetManager.hpp"
#include "../globalStateHandler.hpp"
#include "utils/utils.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define IMG_MENU_TAB_TEXTURE_PATH "../assets/image-menu-tab.png"
#define IMG_MENU_TAB_SELECTED_TEXTURE_PATH "../assets/image-menu-tab-selected.png"
#define TRAINER_MENU_TAB_TEXTURE_PATH "../assets/trainer-menu-tab.png"
#define TRAINER_MENU_TAB_SELECTED_TEXTURE_PATH "../assets/trainer-menu-tab-selected.png"

extern Managers::AssetManager glblAssetManager;
extern globalStateHandler* stateHandler;

extern SDL_Renderer* rend;
extern TTF_Font* appFont;

static SDL_Texture* image_menu_tab_texture = nullptr;
static SDL_Texture* image_menu_tab_selected_texture = nullptr;
static SDL_Texture* trainer_sprite_menu_tab_texture = nullptr;
static SDL_Texture* trainer_sprite_menu_tab_selected_texture = nullptr;

static SDL_Texture* imgMenuText = NULL;
static SDL_Color white = {255, 255, 255, 255};
namespace Menu {

    int8_t UsableImage::SetImage(SDL_Texture* tex) {
        this->texture = tex;
        return 0;
    }

    void UsableImage::SelectImage(Map::MapViewport* vp) {
        // Logic for selecting this specific image

        if(stateHandler && stateHandler->imageMenu) {
            for(auto& img : stateHandler->imageMenu->images) {
                if(img.second.is_selected) {
                    img.second.border_color = nullptr;
                    img.second.is_selected = false;
                }
            }
        }

        this->border_color = &white;
        this->is_selected = true;

        if(vp->selected_tiles.empty()) return;

        SDL_Texture* texture = glblAssetManager.GetAsset(this->fpath);
        if(!texture) return;
        
        for(auto tile : vp->selected_tiles) {
            tile.second->texture = texture;
            tile.second->textureId = this->texture_id;
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
        this->rect.y = h - vh_size_t(20.0f) - 75;
        this->rect.w = (int32_t) this->width;
        this->rect.h = (int32_t) this->height;

        if(!image_menu_tab_texture) {
            SDL_Surface* surf = IMG_Load(Utils::getAssetPath(IMG_MENU_TAB_TEXTURE_PATH).c_str());

            w = surf->w;
            h = surf->h;

            image_menu_tab_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        if(!image_menu_tab_selected_texture) {
            SDL_Surface* surf = IMG_Load(Utils::getAssetPath(IMG_MENU_TAB_SELECTED_TEXTURE_PATH).c_str());
            image_menu_tab_selected_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        if(!trainer_sprite_menu_tab_texture) {
            SDL_Surface* surf = IMG_Load(Utils::getAssetPath(TRAINER_MENU_TAB_TEXTURE_PATH).c_str());
            trainer_sprite_menu_tab_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        if(!trainer_sprite_menu_tab_selected_texture) {
            SDL_Surface* surf = IMG_Load(Utils::getAssetPath(TRAINER_MENU_TAB_SELECTED_TEXTURE_PATH).c_str());
            trainer_sprite_menu_tab_selected_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        SDL_Rect img_menu_tab = { .x = this->rect.x, .y = this->rect.y - h, .w = w, .h = h};
        this->tabs.push_back(img_menu_tab);
        img_menu_tab.x += w;
        this->tabs.push_back(img_menu_tab);
    }

    ImageMenu::~ImageMenu() {
        if (imgMenuText) {
            SDL_DestroyTexture(imgMenuText);
            imgMenuText = NULL;
        }

        for (SDL_Surface* surface : this->raw_surfaces) {
            if (surface != nullptr) {
                SDL_FreeSurface(surface);
            }
        }

        if(image_menu_tab_texture) {
            SDL_DestroyTexture(image_menu_tab_texture);
            image_menu_tab_texture = NULL;
        }
        if(image_menu_tab_selected_texture) {
            SDL_DestroyTexture(image_menu_tab_selected_texture);
            image_menu_tab_selected_texture = NULL;
        }
        if(trainer_sprite_menu_tab_texture) {
            SDL_DestroyTexture(trainer_sprite_menu_tab_texture);
            trainer_sprite_menu_tab_texture = NULL;
        }
        if(trainer_sprite_menu_tab_selected_texture) {
            SDL_DestroyTexture(trainer_sprite_menu_tab_selected_texture);
            trainer_sprite_menu_tab_selected_texture = NULL;
        }

        this->raw_surfaces.clear();
    }

    void ImageMenu::LoadTabs() {
        switch (stateHandler->active_mode) {
            case EditorMode::TILE_PAINT: 
            {
                int32_t w,h;
                SDL_QueryTexture(image_menu_tab_selected_texture, NULL, NULL, &w, &h);

                SDL_Rect dst = { .x = this->rect.x, .y = this->rect.y - h, .w = w, .h = h };
                SDL_RenderCopy(rend, image_menu_tab_selected_texture, NULL, &dst);
 
                /// Images are the same size, no alterations to w,h or any other field of dst necessaty
                dst.x += w;
                SDL_RenderCopy(rend, trainer_sprite_menu_tab_texture, NULL, &dst);

                break;
            }
            case EditorMode::TRAINER_PLACE:
            {
                int32_t w,h;
                SDL_QueryTexture(image_menu_tab_texture, NULL, NULL, &w, &h);

                SDL_Rect dst = { .x = this->rect.x, .y = this->rect.y - h, .w = w, .h = h };
                SDL_RenderCopy(rend, image_menu_tab_texture, NULL, &dst);
 
                /// Images are the same size, no alterations to w,h or any other field of dst necessaty
                dst.x += w;
                SDL_RenderCopy(rend, trainer_sprite_menu_tab_selected_texture, NULL, &dst);
            }
            default:
                break;
        }
    }

    void ImageMenu::Render() {
        SDL_RenderFillRect(rend, &this->rect);

        SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
        SDL_RenderDrawRect(rend, &this->rect);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        if(!imgMenuText) {
            SDL_Surface* surf = TTF_RenderText_Solid(appFont, "Image Menu", {150, 150, 150, 255});
            imgMenuText = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

       this->LoadTabs();

        int32_t w,h;
        SDL_QueryTexture(imgMenuText, NULL, NULL, &w, &h);

        SDL_Rect textRect = {
            .x = (int32_t) (this->rect.x + (this->rect.w * .005f)),
            .y = (int32_t) (this->rect.y + (this->rect.h * .05f)),
            .w = w,
            .h = h
        };

        SDL_RenderCopy(rend, imgMenuText, NULL, &textRect);

        for (auto const& pair : this->images) {
            SDL_RenderCopy(rend, pair.second.texture, NULL, &pair.second.rect);

            if(pair.second.border_color) {
                SDL_Color color = *pair.second.border_color;

                SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
                SDL_RenderDrawRect(rend, &pair.second.rect);
                SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            }
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
            std::string save_path = "";

            /// Handle different Atlas
            if(stateHandler->active_mode == EditorMode::TILE_PAINT) save_path = stateHandler->currentProjectPath + "/tiles.bmp";
            if(stateHandler->active_mode == EditorMode::TRAINER_PLACE) save_path = stateHandler->currentProjectPath + "/trainers.bmp";
            
            if (SDL_SaveBMP(atlas, save_path.c_str()) != 0) {
                std::cerr << "Failed to save sprite sheet: " << SDL_GetError() << std::endl;
            } else {
                std::cout << "Successfully saved atlas to: " << save_path << std::endl;
            }
        } else {
            std::cerr << "Cannot save tiles.bmp: No project is currently open!" << std::endl;
        }

        SaveManifest();
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
            if( !std::filesystem::exists(new_destination) ){
                std::cerr << "Failed to copy imported file: " << e.what() << std::endl;
                return -1;
            }
        }

        /// Checks if the image exists inside the importedImages set that contains the file path of all imported images
        if(this->importedImages.find(new_destination) != this->importedImages.end()) {
            /// The image exists nothing to be done
            return 1;
        };

        SDL_Surface* newSurface = NULL;

        if(original_file.extension() == ".bmp") newSurface = SDL_LoadBMP(new_destination.c_str());
        else newSurface = IMG_Load(new_destination.c_str());

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
        newUiImg.texture_id = this->raw_surfaces.size();

        if( SDL_Texture* text = glblAssetManager.GetAsset(new_destination) ) {
            newUiImg.SetImage(text);
        }

        int32_t current_count = this->raw_surfaces.size() - 1;
        newUiImg.rect.w = this->image_size;
        newUiImg.rect.h = this->image_size;
        newUiImg.rect.x = this->rect.x + (this->rect.w * .005f) + (current_count * (this->image_size + this->menu_padding));
        newUiImg.rect.y = this->rect.y + (this->rect.h * .15f); 

        ImageCoord coord = {(int32_t)this->raw_surfaces.size(), 0};
        this->images[coord] = newUiImg;

        PackAndSaveSpriteSheet();

        this->importedImages.insert(new_destination);

        return 0;
    }

    void ImageMenu::AbsoluteRemoveTexture(Map::MapRenderer* mr) {

        uint16_t selected_texture_id = 0;
        std::string fpath = "";
        ImageCoord key;

        for(const auto& img : this->images) {

            if(img.second.is_selected) {
                selected_texture_id = img.second.texture_id;
                fpath = img.second.fpath;
                key = img.first;
            }
        }

        if(selected_texture_id == 0) return;

        this->images.erase(key);
        this->importedImages.erase(fpath);

        for(auto& tile : mr->grid) {

            if(tile.second.textureId == selected_texture_id) {
                tile.second.DeleteTexture();
            }

            if(tile.second.textureId > selected_texture_id) {
                tile.second.textureId--;
            }
        }

        SDL_FreeSurface(this->raw_surfaces[selected_texture_id - 1]);
        this->raw_surfaces.erase(this->raw_surfaces.begin() + selected_texture_id - 1);

        //glblAssetManager.RemoveAsset(fpath);
        std::remove(fpath.c_str());
        PackAndSaveSpriteSheet();
    }

    void ImageMenu::SaveManifest() {
        if (!stateHandler || stateHandler->currentProjectPath.empty()) {
            std::cerr << "Cannot save manifest: No project is currently open!" << std::endl;
            return;
        }
    
        std::string manifest_path = "";
        if(stateHandler->active_mode == EditorMode::TILE_PAINT) manifest_path = stateHandler->currentProjectPath + "/data/manifest.txt";
        if(stateHandler->active_mode == EditorMode::TRAINER_PLACE) manifest_path = stateHandler->currentProjectPath + "/data/trainer_manifest.txt";
        std::ofstream file(manifest_path);
    
        if (!file.is_open()) {
            std::cerr << "Failed to open manifest for writing: " << manifest_path << std::endl;
            return;
        }
    
        // The images map is sorted by ImageCoord (x = texture_id),
        // so iterating it gives us entries in correct ID order already.
        for (const auto& pair : this->images) {
            file << pair.second.fpath << "\n";
        }
    
        file.close();
        std::cout << "Manifest saved to: " << manifest_path << std::endl;
    }

    void ImageMenu::LoadFromManifest(const std::string& projectPath, SDL_Renderer* rend) {
        std::string manifest_path = "";
        if(stateHandler->active_mode == EditorMode::TILE_PAINT) manifest_path = projectPath + "/data/manifest.txt";
        if(stateHandler->active_mode == EditorMode::TRAINER_PLACE) manifest_path = projectPath + "/data/trainer_manifest.txt";

        std::ifstream file(manifest_path);
    
        if (!file.is_open()) {
            // No manifest exists yet (old project), gracefully do nothing.
            std::cerr << "No manifest found at: " << manifest_path << std::endl;
            return;
        }
    
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // ImportImage assigns IDs sequentially based on raw_surfaces.size(),
                // so calling it in manifest order gives the exact same IDs as when saved.
                this->ImportImage(line, rend);
            }
        }
    
        file.close();
    }


}
