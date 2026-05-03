#include <iostream>

#include "createProjMenu.hpp"
#include "../globalStateHandler.hpp"

extern SDL_Renderer* rend;
extern TTF_Font* appFont;
extern globalStateHandler* stateHandler;

namespace Menu {

    CreateProjMenu::CreateProjMenu(int32_t x, int32_t y, float wPercent, float hPercent)
        : w(wPercent), h(hPercent) {

        this->x = x;
        this->y = y;

        this->rect = SDL_Rect {
            .x = this->x,
            .y = this->y,
            .w = this->w,
            .h = this->h
        };

    }

    void CreateProjMenu::Show() {
        /// Clear hovered buttons for a cleaner look ig
        for(auto& btn : this->btns) {
            if(btn.isHovered) btn.isHovered = false;
        }

        if(stateHandler) stateHandler->isCreateProjMenuOpen = true;
    }

    void CreateProjMenu::Close() {
        if(stateHandler) stateHandler->isCreateProjMenuOpen = false;
    }

    void CreateProjMenu::AddBtn(Menu::MenuBtn btn) {
        if(!this->btns.empty()) {
            std::array<int32_t, 2> coords = this->btns.back().GetBtnCoords();
            vw_size_t width = this->btns.back().GetBtnWidth();

            btn.SetXPos(coords[0] + (int32_t)width);
        }

        btn.rect.x = btn.GetBtnCoords()[0];
        btn.rect.y = btn.GetBtnCoords()[1];

        this->btns.push_back(std::move(btn));
    }

    void CreateProjMenu::Render() {
        if(!appFont) {
            std::cout << "appFont DOES NOT EXIST" << std::endl;
        }

        SDL_SetRenderDrawColor(rend, 15, 15, 15, 255);
        SDL_RenderFillRect(rend, &this->rect);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        for(const auto& btn : this->btns) {
            if(btn.isHovered) {
                SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(rend, 150, 150, 150, 128);
                SDL_RenderFillRect(rend, &btn.rect);
                SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
            }

            if(!btn.textTexture) continue;

            int32_t w, h;
            SDL_QueryTexture(btn.textTexture, NULL, NULL, &w, &h);
            
            SDL_Rect textRect = {
                .x = btn.rect.x + (btn.rect.w / 2) - (w / 2),
                .y = btn.rect.y + (btn.rect.h / 2) - (h / 2),
                .w = w,
                .h = h
            };

            SDL_RenderCopy(rend, btn.textTexture, NULL, &textRect);
        }

    }

}