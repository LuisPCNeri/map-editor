#ifndef __IMG_MENU_H__
#define __IMG_MENU_H__

#include <SDL2/SDL.h>
#include <stdint.h>

#include <string>
#include <map>
#include <functional>

#include "../utils/vp_size_t.hpp"

#define BASE_IMAGE_SIZE 64

extern SDL_Renderer* rend;

namespace Menu {

    static const SDL_Color TEXT_COLOR = {255, 255, 255, 255};
    static const SDL_Color BG_COLOR   = {120, 120, 120, 255};

    class ImageCoord {
        public:
            int32_t x;
            int32_t y;
    };

    class UsableImage {
        public:
            ImageCoord coord;
            SDL_Texture* texture;
            SDL_Rect rect;
        int8_t SetImage(SDL_Texture* texture);
        void SelectImage();
    };
    
    class ImageMenu{

        public:
            std::map<ImageCoord, UsableImage> images;
            vh_size_t height;
            vw_size_t width;
            uint16_t image_size;
            SDL_Rect rect;
        ImageMenu(uint16_t img_size = BASE_IMAGE_SIZE);
        void Render();
        int8_t ImportImage();
    };

    class MenuBtn {
            
        private:
            int32_t x;
            int32_t y;
            vw_size_t w;
            vh_size_t h;
            std::function<void()> onClick;

        public:
            SDL_Texture* textTexture;
            SDL_Rect rect;
            bool isHovered;
        MenuBtn(int32_t x, int32_t y, float wPercent, float hPercent, std::string text, std::function<void()> onClickHandler);
        ~MenuBtn();
        void onPress();

        // Delete copy operations to prevent dangling pointers
        MenuBtn(const MenuBtn&) = delete;
        MenuBtn& operator=(const MenuBtn&) = delete;

        MenuBtn(MenuBtn&& other) noexcept;
        MenuBtn& operator=(MenuBtn&& other) noexcept;
        std::array<int32_t, 2> GetBtnCoords();
        vw_size_t GetBtnWidth();
        vh_size_t GetBtnHeight();
        void SetXPos(int32_t x);
        void SetYPos(int32_t y);
    };

}

#endif