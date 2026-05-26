#ifndef __CREATEPROJMENU_H__
#define __CREATEPROJMENU_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>

#include "../engine/menu.hpp"

namespace Menu {

    class CreateProjMenu {
        
        private:
            SDL_Rect rect;
            void RenderText();

        public:
            std::vector<Menu::MenuBtn> btns;
            SDL_Rect textBox;
            std::string text;
            int32_t x;
            int32_t y;
            vw_size_t w;
            vh_size_t h;
            bool isHovered;
            bool isTextBoxActive;

        CreateProjMenu(int32_t x, int32_t y, float wPercent, float hPercent);
        void Show();
        void Close();
        void AddBtn(Menu::MenuBtn btn);
        void SetText(std::string text);
        void AddText(std::string text);
        void Render();
        bool CheckTextBoxIsHovered(int32_t mouse_x, int32_t mouse_y);

    };

}

#endif