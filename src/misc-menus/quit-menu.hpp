#ifndef __QUIT_MENU_H__
#define __QUIT_MENU_H__

#include "../engine/menu.hpp"

namespace Menu {

    class QuitMenu {
        private:
            SDL_Rect rect;
            bool is_open = false;
        public:
            std::vector<MenuBtn> btns;
        QuitMenu(bool* is_running);
        void Render();

        void Open() {
            this->is_open = true;
        }

        bool IsOpen() {
            return this->is_open;
        }

        void Close() {
            this->is_open = false;
        }
    };

}

#endif