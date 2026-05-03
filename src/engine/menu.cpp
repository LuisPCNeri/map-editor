#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <string>
#include <iostream>

#include "menu.hpp"

extern SDL_Renderer* rend;
extern TTF_Font* appFont;

namespace Menu {

    MenuBtn::MenuBtn(int32_t x, int32_t y, float wPercent, float hPercent, std::string text, std::function<void()> onClickHandler)
        : w(wPercent), h(hPercent) {

        this->x         = x;
        this->y         = y;
        this->isHovered = false;
        this->onClick   = onClickHandler;
        
        if (appFont && rend) {
            SDL_Surface* textSurf = TTF_RenderText_Solid(appFont, text.c_str(), Menu::TEXT_COLOR);
            this->textTexture = SDL_CreateTextureFromSurface(rend, textSurf);
            SDL_FreeSurface(textSurf);
        } else {
            this->textTexture = nullptr;
            std::cerr << "MenuBtn Error: appFont or Renderer is NULL" << std::endl;
        }

        this->rect = SDL_Rect {
            .x = this->x,
            .y = this->y,
            .w = this->w,
            .h = this->h
        };
    }

    MenuBtn::~MenuBtn() {
        if(this->textTexture) SDL_DestroyTexture(this->textTexture);
    }

    void MenuBtn::onPress() {
        if(this->onClick) this->onClick();
    }

    MenuBtn::MenuBtn(MenuBtn&& other) noexcept 
        : x(other.x),
          y(other.y),
          w(other.w),
          h(other.h),
          onClick(std::move(other.onClick)),
          textTexture(other.textTexture),
          rect(other.rect),
          isHovered(other.isHovered) {

        other.textTexture = nullptr;
    }

    MenuBtn& MenuBtn::operator=(MenuBtn&& other) noexcept {
        if(this != &other) {
            if(textTexture) SDL_DestroyTexture(textTexture);

            textTexture = other.textTexture;
            rect        = other.rect;
            x           = other.x;
            y           = other.y;
            w           = other.w;
            h           = other.h;
            onClick     = std::move(other.onClick);
            isHovered   = other.isHovered;

            other.textTexture = nullptr;
        }

        return *this;
    }

    std::array<int32_t,2> MenuBtn::GetBtnCoords() {
        return {this->x, this->y};
    }

    vw_size_t MenuBtn::GetBtnWidth() {
        return this->w;
    }

    vh_size_t MenuBtn::GetBtnHeight() {
        return this->h;
    }

    void MenuBtn::SetXPos(int32_t x){
        this->x = x;
    }

    void MenuBtn::SetYPos(int32_t y){
        this->y = y;
    }
}