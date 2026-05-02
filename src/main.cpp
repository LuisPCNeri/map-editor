#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "engine/map.hpp"
#include "engine/menu.hpp"
#include "toolbar/toolbar.hpp"

#define FONT_DIRECTORY "fonts/arial.ttf"

SDL_Renderer* rend = NULL;
TTF_Font* appFont = NULL;
bool is_running = true;

static void SetUpToolbar(Toolbar::Toolbar* toolbar){
    std::string saveStr = "Save";
    Toolbar::ToolbarBtn saveBtn(saveStr);

    std::string openStr = "Open";
    Toolbar::ToolbarBtn openBtn(openStr);

    toolbar->ToolbarAddBtn(std::move(saveBtn));
    toolbar->ToolbarAddBtn(std::move(openBtn));
    return;
}

int main(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0 ){
        std::cerr << "Error SDL_Init";
        exit(EXIT_FAILURE);
    }
    if(TTF_Init() < 0) {
        std::cerr << TTF_GetError() << "\n";
        exit(EXIT_FAILURE);
    }

    appFont = TTF_OpenFont(FONT_DIRECTORY, 16);

    SDL_Rect usable_bounds;
    if(SDL_GetDisplayBounds(0, &usable_bounds) != 0){
        std::cerr << "Error getting display bounds: " << SDL_GetError() << std::endl; 
    }

    SDL_Window* window = SDL_CreateWindow("Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        usable_bounds.w, usable_bounds.h, SDL_WINDOW_MAXIMIZED);

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

    int32_t screenW, screenH;
    SDL_GetRendererOutputSize(rend, &screenW, &screenH);

    Map::MapRenderer map_rend;
    Map::MapViewport viewport;

    Menu::ImageMenu img_menu;
    Toolbar::Toolbar toolbar;

    SetUpToolbar(&toolbar);
    
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
                    if(event.motion.y > toolbar.y + toolbar.height && event.motion.y < screenH - img_menu.height )
                        viewport.is_mouse_down = true;

                    last_mpos_x = event.motion.x;
                    last_mpos_y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    viewport.is_mouse_down = false;
                    if(event.motion.y < toolbar.y + toolbar.height || event.motion.y > screenH - img_menu.height )
                        break;
                    {
                        bool mouse_moved_x = event.motion.x - last_mpos_x != 0;
                        bool mouse_moved_y = event.motion.y - last_mpos_y != 0;

                        if(!mouse_moved_x && !mouse_moved_y){
                            map_rend.SelectTile(last_mpos_x, last_mpos_y, &viewport);
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    for(auto& btn : toolbar.btns) {
                        btn.isHovered = (event.motion.x > btn.rect.x && 
                                         event.motion.x < btn.rect.x + btn.rect.w &&
                                         event.motion.y > btn.rect.y && 
                                         event.motion.y < btn.rect.y + btn.rect.h);
                    }

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
                case SDL_KEYDOWN:
                    if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
                        viewport.ctrl_down = true;
                    break;
                case SDL_KEYUP:
                    if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
                        viewport.ctrl_down = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);

        map_rend.RenderVisible(&viewport);
        img_menu.Render();
        toolbar.Render();
        SDL_RenderPresent(rend);
    }

    TTF_CloseFont(appFont);
    TTF_Quit();
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}