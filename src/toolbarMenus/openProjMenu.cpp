#include <iostream>
#include <filesystem>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../globalStateHandler.hpp"
#include "openProjMenu.hpp"

extern SDL_Renderer* rend;
extern TTF_Font* appFont;

#define BASE_PROJ_FOLDER ".MapMakerProjects"

namespace Menu {

    OpenProjMenu::OpenProjMenu(int32_t x, int32_t y, float hPercent, float wPercent) {

        vh_size_t height(hPercent);
        vw_size_t width(wPercent);

        this->dropdown = {
            .x = x,
            .y = y,
            .w = width,
            .h = height
        };

    }

    std::vector<std::string> OpenProjMenu::GetExistingProjects() {
        std::vector<std::string> dirs;

        const char* homeDir = std::getenv("HOME");
        if (!homeDir) {
            std::cerr << "CRITICAL ERROR: Could not find HOME environment variable!" << std::endl;
            exit(EXIT_FAILURE);
        }

        struct stat sb;
        std::string path = std::string(homeDir) + "/" + BASE_PROJ_FOLDER + "/";

        for(const auto& entry : std::filesystem::directory_iterator(path)) {
            std::filesystem::path outfilename = entry.path();
            std::string outfilename_str = outfilename.string();

            const char* path_char_ptr = outfilename_str.c_str();
            if (stat(path_char_ptr, &sb) == 0 && sb.st_mode & S_IFDIR){
                dirs.push_back(outfilename_str);
            }
        }

        return dirs;
    }

    void OpenProjMenu::OpenMenu() {
        this->isMenuOpen = true;
    }
    
    void OpenProjMenu::CloseMenu() {
        this->isMenuOpen = false;
    }

    void OpenProjMenu::AddBtn(Menu::MenuBtn btn) {
        int32_t padding = 10;

        if(!this->btns.empty()) {
            Menu::MenuBtn& lastBtn = this->btns.back();
            
            btn.rect.x = this->dropdown.x;
            btn.rect.y = lastBtn.rect.y + lastBtn.GetBtnHeight() + padding;
            btn.rect.w = this->dropdown.w;

            this->btns.push_back(std::move(btn));
        } 
        else {
            btn.rect.x = this->dropdown.x;
            btn.rect.y = this->dropdown.y + padding;
            btn.rect.w = this->dropdown.w;
            
            this->btns.push_back(std::move(btn));
        }

        this->dropdown.h = (this->btns.size() * this->btns.back().GetBtnHeight()) + ((this->btns.size() + 1) * padding);
    }

    void OpenProjMenu::Render() {
        if(!this->isMenuOpen) return;
        
        if(!appFont) {
            std::cerr << "App Font does not exist." << std::endl;
            return;
        }

        SDL_SetRenderDrawColor(rend, 15, 15, 15, 255);
        SDL_RenderFillRect(rend, &this->dropdown);
        SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
        SDL_RenderDrawRect(rend, &this->dropdown);
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
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        }
    }

}