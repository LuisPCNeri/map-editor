#include <SDL2/SDL.h>
#include <iostream>
#include <map.hpp>

SDL_Renderer* rend = NULL;
bool is_running = true;

int main(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0 ){
        std::cerr << "Error SDL_Init";
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow("Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        380, 380, SDL_WINDOW_MAXIMIZED);

    if (!window){
        std::cout << "Error Creating Window";
        exit(EXIT_FAILURE);
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC;
    rend = SDL_CreateRenderer(window, -1, render_flags);
    if(!rend){
        std::cout << SDL_GetError();
        exit(EXIT_FAILURE);
    }

    Map::MapRenderer map_rend;
    Map::MapViewport viewport;
    
    int32_t last_mpos_x = 0;
    int32_t last_mpos_y = 0;

    map_rend.grid[Map::TileCoord(0,0)] = Map::Tile(Map::TileCoord(0,0), viewport.tile_size);
    map_rend.grid[Map::TileCoord(0,0)].border_color = {255, 0, 0, 255};

    while(is_running){

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) is_running = false;

            switch(event.type){
                case SDL_MOUSEBUTTONDOWN:
                    viewport.is_mouse_down = true;

                    last_mpos_x = event.motion.x;
                    last_mpos_y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    viewport.is_mouse_down = false;
                    {
                        bool mouse_moved_x = event.motion.x - last_mpos_x != 0;
                        bool mouse_moved_y = event.motion.y - last_mpos_y != 0;

                        if(!mouse_moved_x && !mouse_moved_y){
                            map_rend.SelectTile(last_mpos_x, last_mpos_y, &viewport);
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if(viewport.is_mouse_down){
                        int32_t tx = (event.motion.x - last_mpos_x) / viewport.tile_size;
                        int32_t ty = (event.motion.y - last_mpos_y) / viewport.tile_size;

                        if (tx != 0 || ty != 0) {
                            viewport.Drag(tx, ty);
                            
                            last_mpos_x += tx * viewport.tile_size;
                            last_mpos_y += ty * viewport.tile_size;
                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);

        map_rend.RenderVisible(&viewport);
        SDL_RenderPresent(rend);
    }

    return 0;
}