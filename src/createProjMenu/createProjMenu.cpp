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

        this->textBox = SDL_Rect {
            .x = (int32_t) (this->x + (this->w * .1f)),
            .y = (int32_t) (this->y + (this->h * .3f)),
            .w = (int32_t) (this->w * .85f),
            .h = (int32_t) (this->h * .25f)
        };

        this->text = "";
        this->isHovered = false;
        this->isTextBoxActive = false;
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

    void CreateProjMenu::SetText(std::string text) {
        this->text = text;
    }

    void CreateProjMenu::RenderText() {
        SDL_Color white = {255, 255, 255, 255};

        SDL_Surface* surface = TTF_RenderText_Solid(appFont, this->text.c_str(), white);
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

            SDL_SetRenderDrawColor(rend, 69, 69, 69, 255);
            SDL_RenderFillRect(rend, &this->textBox);
            SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
            SDL_RenderDrawRect(rend, &this->textBox);
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

            RenderText();
        }

    }

    bool CreateProjMenu::CheckTextBoxIsHovered(int32_t x, int32_t y) {

        return (x > this->textBox.x && 
                x < this->textBox.x + this->textBox.w &&
                y > this->textBox.y && 
                y < this->textBox.y + this->textBox.h);

    }
}