#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <functional>
#include "../utils/vp_size_t.hpp"

namespace Toolbar {

    class ToolbarBtn {
        public:
            std::string text;
            int32_t x, y;
            SDL_Texture* textTexture;
            bool isHovered = false;

            vh_size_t height;
            vw_size_t width;

            SDL_Rect rect;

            std::function<void()> OnClick;
    
        ToolbarBtn(int32_t x = 0, int32_t y = 0, float height_percent = 5.0f, float width_percent = 10.0f, std::string btnText = "");
        ToolbarBtn(std::string btnText);
        
        ToolbarBtn(ToolbarBtn&& other) noexcept;
        ToolbarBtn& operator=(ToolbarBtn&& other) noexcept;

        ~ToolbarBtn();
        void OnMouseHover();
        void OnPress();
    };

    class Toolbar {
        public:
            std::vector<ToolbarBtn> btns;
            int32_t x, y;

            vh_size_t height;
            vw_size_t width;

            SDL_Rect rect;

        Toolbar(int32_t x = 0, int32_t y = 0, float height_percent = 5.0f, float width_percent = 100);
        ~Toolbar();
        void ToolbarAddBtn(ToolbarBtn btn);
        std::array<int32_t, 2> GetLastBtnCoords();
        void Render();
    };
}

#endif