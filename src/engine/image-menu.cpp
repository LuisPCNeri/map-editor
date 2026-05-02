#include "menu.hpp"

namespace Menu {

    int8_t UsableImage::SetImage(SDL_Texture* tex) {
        this->texture = tex;
        return 0;
    }

    void UsableImage::SelectImage() {
        // Logic for selecting this specific image
    }

    ImageMenu::ImageMenu(uint16_t img_size)
        : height(20.0f), width(100.0f) {
        
        int32_t w, h;
        SDL_GetRendererOutputSize(rend, &w, &h);

        this->image_size = img_size;
        this->rect.x = 0;
        this->rect.y = h - this->height;
        this->rect.w = (int32_t) this->width;
        this->rect.h = (int32_t) this->height;
    }

    void ImageMenu::Render() {
        SDL_RenderFillRect(rend, &this->rect);
    }

    int8_t ImageMenu::ImportImage() {
        return 0;
    }

}