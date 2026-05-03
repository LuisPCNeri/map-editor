#include <iostream>
#include <stdint.h>
#include <SDL2/SDL_ttf.h>

#include "toolbar.hpp"
#include "../utils/vp_size_t.hpp"

extern TTF_Font* appFont;
extern SDL_Renderer* rend;

namespace Toolbar {
    
    Toolbar::Toolbar(int32_t x, int32_t y, float height_percent, float width_percent) 
        : height(height_percent), width(width_percent) {

        this->x = x;
        this->y = y;

        this->rect = SDL_Rect {
            .x = this->x,
            .y = this->y,
            .w = (int32_t) this->width,
            .h = (int32_t) this->height
        };
    }

    Toolbar::~Toolbar() {
    }

    std::array<int32_t, 2> Toolbar::GetLastBtnCoords() {
        const ToolbarBtn& btn = this->btns.back();
        std::array<int32_t, 2> coords = {btn.x, btn.y};

        return coords;
    }

    void Toolbar::ToolbarAddBtn(ToolbarBtn btn) {

        if(!this->btns.empty()) {
            std::array<int32_t, 2> lastBtnCoords = this->GetLastBtnCoords();
            btn.x = lastBtnCoords[0] + (int32_t)this->btns.back().width;
        }

        btn.rect.x = btn.x;
        btn.rect.y = btn.y;

        this->btns.push_back(std::move(btn));
    }

    void Toolbar::Render() {
        if(!appFont) {
            std::cout << "appFont DOES NOT EXIST" << std::endl; 
        }

        SDL_SetRenderDrawColor(rend, 15, 15, 15, 255);
        SDL_RenderFillRect(rend, &this->rect);

        for( const auto& btn : this->btns ) {
            if (btn.isHovered) {
                SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(rend, 150, 150, 150, 128);
                SDL_RenderFillRect(rend, &btn.rect);
                SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
            }

            if (!btn.textTexture) 
                continue;
            
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

    ToolbarBtn::ToolbarBtn(int32_t x, int32_t y, float height_percent, float width_percent, std::string btnText)
        : height(height_percent), width(width_percent) {

        const SDL_Color textColor = {255, 255, 255, 255};
        
        this->x      = x;
        this->y      = y;
        this->text   = btnText;

        this->rect = SDL_Rect {
            .x = this->x,
            .y = this->y,
            .w = (int32_t) this->width,
            .h = (int32_t) this->height  
        };

        SDL_Surface* textSurf    = TTF_RenderText_Solid(appFont, this->text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(rend, textSurf);
        this->textTexture = textTexture;

        SDL_FreeSurface(textSurf);
    }

    ToolbarBtn::ToolbarBtn(std::string btnText)
        : height(5.0f), width(5.0f) {
        
        const SDL_Color textColor = {255, 255, 255, 255};
        this->x = 0;
        this->y = 0;
        this->text = btnText;

        this->rect = SDL_Rect {
            .x = this->x,
            .y = this->y,
            .w = (int32_t) this->width,
            .h = (int32_t) this->height  
        };
        
        SDL_Surface* textSurf    = TTF_RenderText_Solid(appFont, this->text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(rend, textSurf);
        this->textTexture = textTexture;

        SDL_FreeSurface(textSurf);
    }

    ToolbarBtn::ToolbarBtn(ToolbarBtn&& other) noexcept 
        :   text(std::move(other.text)), 
            x(other.x),
            y(other.y), 
            textTexture(other.textTexture), 
            isHovered(other.isHovered),
            height(other.height), 
            width(other.width), 
            rect(other.rect), 
            OnClick(std::move(other.OnClick)) {
        other.textTexture = nullptr; 
    }

    ToolbarBtn& ToolbarBtn::operator=(ToolbarBtn&& other) noexcept {
        if (this != &other) {

            if (textTexture) 
                SDL_DestroyTexture(textTexture);

            text        = std::move(other.text);
            x           = other.x;
            y           = other.y;
            textTexture = other.textTexture;
            isHovered   = other.isHovered;
            height      = other.height;
            width       = other.width;
            rect        = other.rect;
            OnClick     = std::move(other.OnClick);
            other.textTexture = nullptr;
        }
        return *this;
    }

    ToolbarBtn::~ToolbarBtn() {
        if (this->textTexture) {
            SDL_DestroyTexture(this->textTexture);
            this->textTexture = nullptr;
        }
    }

    void ToolbarBtn::OnPress() {
        if(this->OnClick) {
            this->OnClick();
        }
    }
}