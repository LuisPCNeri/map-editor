#include "iostream"

#include "menu.hpp"
#include "../globalStateHandler.hpp"
#include <SDL2/SDL_ttf.h>


extern SDL_Renderer* rend;
extern TTF_Font* appFont;
extern globalStateHandler* stateHandler;

static SDL_Texture* spawn_id_menu_texture = NULL;

namespace Menu {
    
    void SpawnIdMenu::ApplyChanges() {
        if(!stateHandler || !stateHandler->mapRenderer) return;

        std::cout << "APPLY CHANGES!" << std::endl;

        std::vector<int16_t> ids;
        std::string token;

        for(char c : this->inp_text) {
            if(c == ',' || c == ' ') {
                if(!token.empty()) {
                    try { ids.push_back((int16_t)std::stoi(token)); } catch(...) {}
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if(!token.empty()) {
            try { ids.push_back((int16_t)std::stoi(token)); } catch(...) {}
        }

        stateHandler->mapRenderer->SetSpawnIdsByTexture(this->target_texture_id, ids);
        this->Close();
        delete stateHandler->spawnIdMenu;
        stateHandler->spawnIdMenu = nullptr;
    }

    SpawnIdMenu::SpawnIdMenu(int32_t x, int32_t y, float w_percent, float h_percent)
        : appl_btn(0, 0, 5.0f, 1.0f, "Apply Changes", [this]() { this->ApplyChanges(); }) {
        this->rect = SDL_Rect {
            .x = x,
            .y = y,
            .w = vw_size_t(w_percent),
            .h = vh_size_t(h_percent)
        };

        int32_t tbW = (int32_t)(this->rect.w * 0.85f);
        int32_t tbH = (int32_t)(this->rect.h * 0.15f);

        this->textBox = SDL_Rect {
            .x = this->rect.x + (this->rect.w - tbW) / 2,
            .y = (int32_t)(this->rect.y + (this->rect.h * 0.4f)),
            .w = tbW,
            .h = tbH
        };

        int32_t w = vw_size_t(5.0f);
        int32_t h = vh_size_t(1.0f);

        this->appl_btn.rect.x = this->rect.x + (this->rect.w / 2) - (w/2);
        this->appl_btn.rect.y = this->rect.y + this->rect.h - h - (int32_t)(this->rect.h * .15f);
    }

    SpawnIdMenu::SpawnIdMenu(int32_t x, int32_t y, float w_percent, float h_percent, uint16_t target_texture_id)
        : appl_btn(0, 0, 5.0f, 1.0f, "Apply Changes", [this]() { this->ApplyChanges(); }) {
        this->rect = SDL_Rect {
            .x = x,
            .y = y,
            .w = vw_size_t(w_percent),
            .h = vh_size_t(h_percent)
        };

        int32_t tbW = (int32_t)(this->rect.w * 0.85f);
        int32_t tbH = (int32_t)(this->rect.h * 0.15f);

        this->textBox = SDL_Rect {
            .x = this->rect.x + (this->rect.w - tbW) / 2,
            .y = (int32_t)(this->rect.y + (this->rect.h * 0.4f)),
            .w = tbW,
            .h = tbH
        };

        int32_t w = vw_size_t(5.0f);
        int32_t h = vh_size_t(1.0f);

        this->appl_btn.rect.x = this->rect.x + (this->rect.w / 2) - (w/2);
        this->appl_btn.rect.y = this->rect.y + this->rect.h - h - (int32_t)(this->rect.h * .15f);

        this->target_texture_id = target_texture_id;
    }

    void SpawnIdMenu::RenderText() {
        SDL_Color white = {255, 255, 255, 255};

        SDL_Surface* surface = TTF_RenderText_Solid(appFont, this->inp_text.c_str(), white);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(rend, surface);

        int32_t w, h;
        SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);

        SDL_Rect textRect = {
            .x = (int32_t) (this->textBox.x + (this->textBox.w * .02f) ),
            .y = (int32_t) (this->textBox.y + (this->textBox.h / 2) - (h / 2)),
            .w = w,
            .h = h
        };

        SDL_RenderCopy(rend, textTexture, NULL, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(textTexture);
    }

    bool SpawnIdMenu::TextBoxIsHovered(int32_t x, int32_t y) {
        return (x > this->textBox.x && 
                x < this->textBox.x + this->textBox.w &&
                y > this->textBox.y && 
                y < this->textBox.y + this->textBox.h);
    }

    void SpawnIdMenu::Render() {

        if(!appFont) {
            std::cout << "appFont DOES NOT EXIST" << std::endl;
        }

        SDL_SetRenderDrawColor(rend, 15, 15, 15, 255);
        SDL_RenderFillRect(rend, &this->rect);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        if(this->appl_btn.isHovered) {
            SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(rend, 150, 150, 150, 128);

            SDL_Rect hover_rect = {
                .x = this->appl_btn.rect.x - 20,
                .y = this->appl_btn.rect.y - 20,
                .w = this->appl_btn.rect.w + 40,
                .h = this->appl_btn.rect.h + 40,
            };

            SDL_RenderFillRect(rend, &hover_rect);
            SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
        }

        if(!spawn_id_menu_texture) {
            SDL_Surface* surf = TTF_RenderText_Solid(appFont, "Spawn Id Pool Menu", {150, 150, 150, 255});
            spawn_id_menu_texture = SDL_CreateTextureFromSurface(rend, surf);
            SDL_FreeSurface(surf);
        }

        int32_t w,h;
        SDL_QueryTexture(spawn_id_menu_texture, NULL, NULL, &w, &h);

        SDL_Rect title_text_rect = {
            .x = (int32_t) (this->rect.x + (this->rect.w * .05f)),
            .y = (int32_t) (this->rect.y + (this->rect.h * .05f)),
            .w = w,
            .h = h
        };

        SDL_RenderCopy(rend, spawn_id_menu_texture, NULL, &title_text_rect);

        if(!this->appl_btn.textTexture) return;

        SDL_QueryTexture(this->appl_btn.textTexture, NULL, NULL, &w, &h);
        
        SDL_Rect textRect = {
            .x = this->appl_btn.rect.x + (this->appl_btn.rect.w / 2) - (w / 2),
            .y = this->appl_btn.rect.y + (this->appl_btn.rect.h / 2) - (h / 2),
            .w = w,
            .h = h
        };

        SDL_RenderCopy(rend, this->appl_btn.textTexture, NULL, &textRect);

        SDL_SetRenderDrawColor(rend, 69, 69, 69, 255);
        SDL_RenderFillRect(rend, &this->textBox);
        SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
        SDL_RenderDrawRect(rend, &this->textBox);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        RenderText();

        //std::cout << this->inp_text << std::endl;
    }
}