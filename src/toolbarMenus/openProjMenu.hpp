#ifndef __OPEN_PROJECT_MENU_H__
#define __OPEN_PROJECT_MENU_H__

#include "../engine/menu.hpp"

namespace Menu {


    class OpenProjMenu {
        public:
            SDL_Rect dropdown;
            std::vector<Menu::MenuBtn> btns;
            bool isMenuOpen = false;
            bool isHovered = false;

            OpenProjMenu(int32_t x, int32_t y, float hPercent, float wPercent);
            static void OpenProject();
            static std::vector<std::string> GetExistingProjects();
            void AddBtn(Menu::MenuBtn btn);
            void Render();

            void CloseMenu();
            void OpenMenu();
    };  

}

#endif