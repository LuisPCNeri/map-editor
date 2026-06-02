#ifndef __IMG_MENU_H__
#define __IMG_MENU_H__

#include <SDL2/SDL.h>
#include <stdint.h>

#include <string>
#include <map>
#include <functional>
#include <set>

#include "../utils/vp_size_t.hpp"
#include "map.hpp"

#define BASE_IMAGE_SIZE 32

extern SDL_Renderer* rend;

namespace Menu {

    static const SDL_Color TEXT_COLOR = {255, 255, 255, 255};
    static const SDL_Color BG_COLOR   = {120, 120, 120, 255};

    class ImageCoord {
        public:
            int32_t x;
            int32_t y;

        bool operator<(const ImageCoord& other) const {
            if (x != other.x) {
                return x < other.x;
            }
            return y < other.y;
        }
    };

    class UsableImage {
        public:
            ImageCoord coord;
            std::string fpath = "";

            uint16_t texture_id;

            SDL_Color* border_color = nullptr;

            SDL_Texture* texture;
            SDL_Rect rect;
            bool isHovered = 0;
            bool is_selected = false;
        int8_t SetImage(SDL_Texture* texture);
        void SelectImage(Map::MapViewport* viewport);
    };
    
    class ImageMenu{

        public:
            std::map<ImageCoord, UsableImage> images;
            std::vector<SDL_Surface*> raw_surfaces;
            std::set<std::string> importedImages;

            uint32_t menu_padding = 8; // px

            vh_size_t height;
            vw_size_t width;
            uint16_t image_size;
            SDL_Rect rect;
        ImageMenu(uint16_t img_size = BASE_IMAGE_SIZE);
        ~ImageMenu();
        void Render();
        int8_t ImportImage(const std::string& filepath, SDL_Renderer* rend);
        void PackAndSaveSpriteSheet();
        void AbsoluteRemoveTexture(Map::MapRenderer* mr);
        void SaveManifest();
        void LoadFromManifest(const std::string& projectPath, SDL_Renderer* rend);

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

    class SpawnIdMenu {
        private:
            SDL_Rect textBox;
            uint16_t target_texture_id;
            bool is_open = false;

            void RenderText();
        public:
            SDL_Rect rect;
            std::string inp_text;
            Menu::MenuBtn appl_btn;

            bool is_textbox_active = false;
        SpawnIdMenu(int32_t x, int32_t y, float w_percent, float h_percent);
        SpawnIdMenu(int32_t x, int32_t y, float w_percent, float h_percent, uint16_t texture_id);

        void Open() {
            this->is_open = true;
        };
        void Close() {
            this->is_open = false;
        };
        bool IsOpen() {
            return this->is_open;
        };
        bool TextBoxIsHovered(int32_t x, int32_t y);

        void ApplyChanges();

        void Render();
    };

}

#endif