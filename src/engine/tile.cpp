#include <SDL2/SDL.h>
#include <iostream>
#include "map.hpp"

extern SDL_Renderer* rend;

namespace Map {

    TileCoord::TileCoord(int32_t x, int32_t y){
        this->x = x;
        this->y = y;
    }

    Tile::Tile(){
        this->texture = nullptr;
        this->rect.x = -1;

        this->border_color = {69, 69, 69, 100};
    }
    Tile::Tile(TileCoord coords, int32_t tile_size){
        this->texture = nullptr;
        this->coord   = coords;

        this->rect.x = coords.x * tile_size;
        this->rect.y = coords.y * tile_size;
        this->rect.w = tile_size;
        this->rect.h = tile_size;

        this->border_color = {69, 69, 69, 100};
    }
    Tile::~Tile(){
        
    }

    void Tile::TileDraw(SDL_Rect dest){
        SDL_SetRenderDrawColor(rend, this->border_color.r, this->border_color.g, this->border_color.b, this->border_color.a);
        SDL_RenderDrawRect(rend, &dest);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
    }

    void Tile::Select() {
        this->border_color = SELECTED_BORDER_COLOR;
    }
    void Tile::DeSelect() {
        this->border_color = BASE_BORDER_COLOR;
    }

}