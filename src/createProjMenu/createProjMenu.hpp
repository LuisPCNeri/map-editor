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
        
        public:
            std::vector<Menu::MenuBtn> btns;
            int32_t x;
            int32_t y;
            vw_size_t w;
            vh_size_t h;

        CreateProjMenu(int32_t x, int32_t y, float wPercent, float hPercent);
        void Show();
        void Close();
        void AddBtn(Menu::MenuBtn btn);
        void Render();

    };

}

#endif