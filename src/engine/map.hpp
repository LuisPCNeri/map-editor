#ifndef __MAP_H__
#define __MAP_H__

#include <SDL2/SDL.h>
#include <map>
#include <vector>
#include <tuple>

#define BASE_TILE_SIZE 32
#define BASE_RENDER_PADDING 5
#define BASE_BORDER_COLOR {69, 69, 69, 100}
#define SELECTED_BORDER_COLOR {0, 255, 0, 255}

namespace Map {
    class TileCoord {
        public:
            int32_t x;
            int32_t y;
        TileCoord(int32_t x = 0, int32_t y = 0);

        bool operator<(const TileCoord& other) const{
            return std::tie(x,y) < std::tie(other.x, other.y);
        }
    };

    class Tile {

        public:
            std::vector<int16_t> spawnable_ids;
            std::string path;

            TileCoord coord;

            SDL_Texture* texture;
            uint16_t textureId;

            SDL_Color border_color;

            SDL_Rect rect;

            uint16_t id;

        Tile();
        Tile(TileCoord coords, int32_t tile_size);
        ~Tile();
        void SetTileSprite(std::string pathToAsset);
        void ResizeTile(uint8_t tile_size);
        void SetTileBorderColor(SDL_Color color);
        void TileDraw(SDL_Rect dest);
        void Select();
        void DeSelect();
    };

    class MapViewport {

        private:
            static constexpr uint8_t ALLOWED_ZOOMS[4] = {16, 32, 64, 128};
            int8_t current_zoom_level = 2;

        public:
            std::map<TileCoord, Tile*> selected_tiles;

            int32_t offsetX;
            int32_t offsetY;

            float zoom;
            bool is_mouse_down;
            bool ctrl_down;
            bool lshift_down = false;
            uint8_t tile_size;
        MapViewport();
        void ZoomIn (int32_t mx, int32_t my);
        void ZoomOut(int32_t mx, int32_t my);
        void ResetZoom();
        void Drag   (int32_t new_mouse_x, int32_t new_mouse_y);
    };

    class MapRenderer {
        public:
            std::map<TileCoord, Tile> grid;
            int16_t render_padding;
        MapRenderer(int16_t render_padding = BASE_RENDER_PADDING);
        void RenderVisible(MapViewport* m);

        int SelectTile(int32_t mouse_x, int32_t mouse_y, MapViewport* m);
        void SelectTileRectangle(MapViewport* m);

        void ExportMapToBin(const std::string& fpath);
        void ImportMapFromBinary(const std::string& fpath);

        void SetSpawnIdsByTexture(uint16_t target_texture_id, std::vector<int16_t> new_ids);
        std::vector<uint16_t> GetSpawnIdsByTexture(uint16_t target_texture_id);
    };
}

#endif