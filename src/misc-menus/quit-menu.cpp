#include <iostream>

#include <SDL2/SDL_ttf.h>

#include "quit-menu.hpp"
#include "../globalStateHandler.hpp"

extern SDL_Renderer* rend;
extern TTF_Font* appFont;
extern globalStateHandler* stateHandler;

namespace Menu {

    QuitMenu::QuitMenu(bool* is_running) {

        int32_t w, h;
        SDL_GetRendererOutputSize(rend, &w, &h);

        vw_size_t rect_w(22.0f);
        vh_size_t rect_h(12.0f);

        this->rect = {
            .x = (w / 2) - (rect_w / 2),
            .y = (h / 2) - (rect_h / 2),
            .w = rect_w, 
            .h = rect_h
        };

        this->btns.push_back(std::move(
            MenuBtn(this->rect.x + this->rect.w - vw_size_t(2.0f) - 5, this->rect.y + 5, 2.0f, 2.0f, "X", [this]() {
                    this->Close();
                }
            ))
        );

        this->btns.push_back(std::move(
            MenuBtn(this->rect.x + 10, this->rect.y + this->rect.h - vh_size_t(4.0f) - 10, 9.0f, 4.0f, "Quit w/o saving", [is_running]{
                if(*is_running) *is_running = false;
            })
        ));

        this->btns.push_back(std::move(
            MenuBtn(this->rect.x + this->rect.w - vw_size_t(9.0f) - 10, this->rect.y + this->rect.h - vh_size_t(4.0f) - 10, 9.0f, 4.0f, "Quit & save", [is_running]{
                if(*is_running) {
                    if(stateHandler->currentProjectPath.empty()) {
                        std::cerr << "MUST HAVE A PROJECT OPEN TO SAVE." << std::endl;
                        return; 
                    }

                    stateHandler->mapRenderer->ExportMapToBin(stateHandler->currentProjectPath + "/data/map.bin");
                    *is_running = false;
                }
            })
        ));
    }

    void QuitMenu::Render() {

        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 150);
        SDL_RenderFillRect(rend, NULL);

        SDL_Rect shadowRect = this->rect;
        shadowRect.x += 6;
        shadowRect.y += 6;
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 100);
        SDL_RenderFillRect(rend, &shadowRect);

        SDL_SetRenderDrawColor(rend, 20, 20, 20, 255);
        SDL_RenderFillRect(rend, &this->rect);
        SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
        SDL_RenderDrawRect(rend, &this->rect);

        for(const auto& btn : this->btns) {
            
            SDL_SetRenderDrawColor(rend, 90, 90, 90, 255);
            SDL_RenderFillRect(rend, &btn.rect);

            if(btn.isHovered) {
                SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
                SDL_RenderFillRect(rend, &btn.rect);
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

            SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
            SDL_RenderCopy(rend, btn.textTexture, NULL, &textRect);
            SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        }
        
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    }

}