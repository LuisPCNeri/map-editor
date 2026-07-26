#include "menu.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <cstdint>

#include "engine/assetManager/assetManager.hpp"
#include "../globalStateHandler.hpp"

#include <filesystem>
#include <fstream>

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

static SDL_Texture* trainer_menu_text = nullptr;

namespace Menu {
    TrainerSpriteMenu::TrainerSpriteMenu() : height(20.0f), width(100.0f) {

        int32_t w, h;
        SDL_GetRendererOutputSize(rend, &w, &h);

        this->rect.x = 0;
        this->rect.y = h - vh_size_t(20.0f) - 75;
        this->rect.w = (int32_t) this->width;
        this->rect.h = (int32_t) this->height;
        if(!image_menu_tab_texture) {
            SDL_Surface* surf = IMG_Load(IMG_MENU_TAB_TEXTURE_PATH);

            w = surf->w;
            h = surf->h;

            image_menu_tab_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        if(!image_menu_tab_selected_texture) {
            SDL_Surface* surf = IMG_Load(IMG_MENU_TAB_SELECTED_TEXTURE_PATH);
            image_menu_tab_selected_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        if(!trainer_sprite_menu_tab_texture) {
            SDL_Surface* surf = IMG_Load(TRAINER_MENU_TAB_TEXTURE_PATH);
            trainer_sprite_menu_tab_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        if(!trainer_sprite_menu_tab_selected_texture) {
            SDL_Surface* surf = IMG_Load(TRAINER_MENU_TAB_SELECTED_TEXTURE_PATH);
            trainer_sprite_menu_tab_selected_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        SDL_Rect img_menu_tab = { .x = this->rect.x, .y = this->rect.y - h, .w = w, .h = h};
        this->tabs.push_back(img_menu_tab);
        img_menu_tab.x += w;
        this->tabs.push_back(img_menu_tab);
    }


    void TrainerSpriteMenu::LoadTabs() {
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

    void TrainerSpriteMenu::Render() {
        SDL_RenderFillRect(rend, &this->rect);

        SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
        SDL_RenderDrawRect(rend, &this->rect);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        if(!trainer_menu_text) {
            SDL_Surface* surf = TTF_RenderText_Solid(appFont, "Trainer Sprite Menu", {150, 150, 150, 255});
            trainer_menu_text = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

       this->LoadTabs();

        int32_t w,h;
        SDL_QueryTexture(trainer_menu_text, NULL, NULL, &w, &h);

        SDL_Rect textRect = {
            .x = (int32_t) (this->rect.x + (this->rect.w * .005f)),
            .y = (int32_t) (this->rect.y + (this->rect.h * .05f)),
            .w = w,
            .h = h
        };

        SDL_RenderCopy(rend, trainer_menu_text, NULL, &textRect);
    }

    TrainerSpriteMenu::~TrainerSpriteMenu() {
        if(image_menu_tab_texture) SDL_DestroyTexture(image_menu_tab_texture);
        if(image_menu_tab_selected_texture) SDL_DestroyTexture(image_menu_tab_selected_texture);
        if(trainer_sprite_menu_tab_texture) SDL_DestroyTexture(trainer_sprite_menu_tab_texture);
        if(trainer_sprite_menu_tab_selected_texture) SDL_DestroyTexture(trainer_sprite_menu_tab_selected_texture);

        if(trainer_menu_text) SDL_DestroyTexture(trainer_menu_text);

        for(auto& surf : this->raw_surfaces) {
           if(surf != nullptr) SDL_FreeSurface(surf); 
        }

        this->raw_surfaces.clear();
    }
}
