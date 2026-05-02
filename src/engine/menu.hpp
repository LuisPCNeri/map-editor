#ifndef __IMG_MENU_H__
#define __IMG_MENU_H__

#include <SDL2/SDL.h>
#include <stdint.h>
#include <map>

#include "../utils/vp_size_t.hpp"

#define BASE_IMAGE_SIZE 64

extern SDL_Renderer* rend;

namespace Menu {

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

}

#endif