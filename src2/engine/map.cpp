#include <SDL2/SDL.h>
#include <iostream>
#include "map.hpp"

extern SDL_Renderer* rend;

namespace Map {

    MapRenderer::MapRenderer(int16_t render_padding){
        this->render_padding = render_padding;
    }

    void MapRenderer::RenderVisible(MapViewport* m){
        int32_t w, h;
        SDL_GetRendererOutputSize(rend, &w, &h);
        int32_t tile_num_x = w / m->tile_size;
        int32_t tile_num_y = h / m->tile_size;

        int32_t start_x = m->offsetX - this->render_padding;
        int32_t start_y = m->offsetY - this->render_padding;

        int32_t end_x = m->offsetX + tile_num_x + this->render_padding;
        int32_t end_y = m->offsetY + tile_num_y + this->render_padding;

        for(auto y = start_y; y < end_y; y++){
            for(auto x = start_x; x < end_x; x++){
                TileCoord coord(x,y);
                
                SDL_Rect dest = {
                    (x - m->offsetX) * m->tile_size,
                    (y - m->offsetY) * m->tile_size,
                    m->tile_size,
                    m->tile_size
                };

                auto tile = this->grid.find(TileCoord(x, y));
                if(tile != this->grid.end()) {
                    tile->second.TileDraw(dest);
                }
                else{
                    Tile new_tile(coord, m->tile_size);
                    auto inserted = this->grid.insert({coord, new_tile});
                    inserted.first->second.TileDraw(dest);
                }
            }
        }

    }

    MapViewport::MapViewport(){
        this->offsetX = 0;
        this->offsetY = 0;

        this->zoom = 0.0f;
        this->tile_size = BASE_TILE_SIZE;

        this->is_mouse_down = false;
    }

    void MapViewport::ZoomIn() {
        
    };

    void MapViewport::ZoomOut() {
        
    }

    void MapViewport::Drag(int32_t new_mouse_x, int32_t new_mouse_y) {
        this->offsetX -= new_mouse_x;
        this->offsetY -= new_mouse_y;
    }

    int MapRenderer::SelectTile(int32_t mouse_x, int32_t mouse_y, MapViewport* m) {
        TileCoord coord((mouse_x / m->tile_size) + m->offsetX, (mouse_y / m->tile_size) + m->offsetY);

        bool are_tiles_selected = !m->selected_tiles.empty();
        if(are_tiles_selected){

            std::map<TileCoord, Tile*>::iterator it;
            for(it = m->selected_tiles.begin(); it != m->selected_tiles.end(); it++){
                it->second->border_color = BASE_BORDER_COLOR;
            }

            m->selected_tiles.clear();
        }

        auto tile = this->grid.find(coord);
        if(tile != this->grid.end()){
            m->selected_tiles.insert({coord, &tile->second});
            tile->second.Select();
            return 1;
        }

        return 0;
    }
}