#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "engine/map.hpp"
#include "engine/menu.hpp"
#include "toolbar/toolbar.hpp"
#include "globalStateHandler.hpp"
#include "toolbar/buttons/toolbarBtn.hpp"

#include "engine/assetManager/assetManager.hpp"

#define FONT_DIRECTORY "fonts/arial.ttf"

SDL_Renderer* rend = NULL;
TTF_Font* appFont = NULL;
bool is_running = true;
globalStateHandler* stateHandler = NULL;

extern char *composition;
extern Sint32 cursor;
extern Sint32 selection_len;

Managers::AssetManager glblAssetManager;

static void SetUpToolbar(Toolbar::Toolbar* toolbar){
    std::string createprojStr = "Create Project";
    Toolbar::ToolbarBtn createProjBtn(0, 0, 5.0f, 8.0f, createprojStr);
    createProjBtn.OnClick = [](){ 
        stateHandler->createProjMenu = CreateProjHandleClick(); 
        if(stateHandler->createProjMenu) stateHandler->createProjMenu->Show();
    };

    std::string saveStr = "Save";
    Toolbar::ToolbarBtn saveBtn(saveStr);
    saveBtn.OnClick = SaveBtnHandleClick;

    std::string openStr = "Open";
    Toolbar::ToolbarBtn openBtn(openStr);
    openBtn.OnClick = [](){
        stateHandler->openProjMenu = OpenBtnHandleClick();
        if(stateHandler->openProjMenu) stateHandler->openProjMenu->OpenMenu();
    };

    toolbar->ToolbarAddBtn(std::move(createProjBtn));
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
    if (!appFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

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

    stateHandler = new globalStateHandler();
    stateHandler->createProjMenu = nullptr;
    stateHandler->isCreateProjMenuOpen = false;

    Map::MapRenderer map_rend;
    Map::MapViewport viewport;

    Menu::ImageMenu img_menu;
    Toolbar::Toolbar toolbar;

    stateHandler->imageMenu = &img_menu;
    stateHandler->mapRenderer = &map_rend;

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
                    if(stateHandler && stateHandler->isCreateProjMenuOpen && stateHandler->createProjMenu) {
                        if(stateHandler->createProjMenu->CheckTextBoxIsHovered(event.button.x, event.button.y)) {
                            SDL_StartTextInput();
                            stateHandler->createProjMenu->isTextBoxActive = true;
                        } else {
                            SDL_StopTextInput();
                            stateHandler->createProjMenu->isTextBoxActive = false;
                        }
                    }

                    if(event.motion.y > toolbar.y + toolbar.height && event.motion.y < img_menu.rect.y )
                        viewport.is_mouse_down = true;

                    last_mpos_x = event.motion.x;
                    last_mpos_y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    viewport.is_mouse_down = false;
                    {
                        bool mouse_moved_x = event.motion.x - last_mpos_x != 0;
                        bool mouse_moved_y = event.motion.y - last_mpos_y != 0;

                        if(event.motion.y < toolbar.y + toolbar.height) {
                            for(auto& btn : toolbar.btns) {
                                if(!btn.isHovered) {
                                    continue;
                                }

                                btn.OnClick();
                                break;
                            }
                            
                            /// Break out of switch statement
                            break;
                        }

                        if(stateHandler->isCreateProjMenuOpen && stateHandler->createProjMenu) {
                            for(auto& btn : stateHandler->createProjMenu->btns) {
                                if(!btn.isHovered) continue;
                                btn.onPress();
                                break;
                            }

                            /// Break out of switch statement
                            break;
                        }

                        if(stateHandler->openProjMenu && stateHandler->openProjMenu->isMenuOpen) {
                            for(auto& btn : stateHandler->openProjMenu->btns) {
                                if(!btn.isHovered) continue;
                                btn.onPress();
                                break;
                            }

                            /// Break out of switch statement
                            break;
                        }
                        else {
                            if(stateHandler->openProjMenu && stateHandler->openProjMenu->isMenuOpen) {
                                stateHandler->openProjMenu->CloseMenu();
                            }
                        }

                        if(event.motion.y > img_menu.rect.y ){
                            for(auto& img : img_menu.images) {
                                if(!img.second.isHovered) continue;

                                img.second.SelectImage(&viewport);
                                break;
                            }

                            break;
                        }

                        if(!mouse_moved_x && !mouse_moved_y){

                            if(viewport.lshift_down && viewport.ctrl_down) {

                                if(viewport.selected_tiles.size() == 1) {
                                    map_rend.SelectTile(last_mpos_x, last_mpos_y, &viewport);
                                    map_rend.SelectTileRectangle(&viewport);
                                    break;
                                }
                            }
                            
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

                    for(auto& img : img_menu.images) {
                        img.second.isHovered = (event.motion.x > img.second.rect.x && 
                                                event.motion.x < img.second.rect.x + img.second.rect.w &&
                                                event.motion.y > img.second.rect.y && 
                                                event.motion.y < img.second.rect.y + img.second.rect.h);
                    }
                    
                    if(stateHandler->isCreateProjMenuOpen && stateHandler->createProjMenu) {
                        for(auto& btn : stateHandler->createProjMenu->btns) {
                            btn.isHovered = (event.motion.x > btn.rect.x && 
                                             event.motion.x < btn.rect.x + btn.rect.w &&
                                             event.motion.y > btn.rect.y && 
                                             event.motion.y < btn.rect.y + btn.rect.h);
                        }
                    }

                    if(stateHandler->openProjMenu && stateHandler->openProjMenu->isMenuOpen) {
                        for(auto& btn : stateHandler->openProjMenu->btns) {
                            btn.isHovered = (event.motion.x > btn.rect.x && 
                                             event.motion.x < btn.rect.x + btn.rect.w &&
                                             event.motion.y > btn.rect.y && 
                                             event.motion.y < btn.rect.y + btn.rect.h);
                        }
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
                    if(event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
                        viewport.lshift_down = true;

                    if(stateHandler && stateHandler->isCreateProjMenuOpen && stateHandler->createProjMenu && stateHandler->createProjMenu->isTextBoxActive) {
                        if(event.key.keysym.sym == SDLK_BACKSPACE && stateHandler->createProjMenu->text.length() > 0) {
                            stateHandler->createProjMenu->text.pop_back();
                        }
                        else if(event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                            SDL_StopTextInput();
                            stateHandler->createProjMenu->isTextBoxActive = false;
                        }
                    }
                break;
                case SDL_KEYUP:
                    if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
                        viewport.ctrl_down = false;
                    if(event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
                        viewport.lshift_down = false;
                break;
                case SDL_TEXTINPUT:
                    if(stateHandler && stateHandler->isCreateProjMenuOpen && stateHandler->createProjMenu && stateHandler->createProjMenu->isTextBoxActive) {
                        stateHandler->createProjMenu->text += event.text.text;
                    }
                break;
                case SDL_DROPFILE: {
                    char* droppedFileDir = event.drop.file;
                    std::string filepath(droppedFileDir);

                    img_menu.ImportImage(filepath, rend);
                    SDL_free(droppedFileDir);
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);

        map_rend.RenderVisible(&viewport);
        img_menu.Render();
        toolbar.Render();

        if(stateHandler->isCreateProjMenuOpen && stateHandler->createProjMenu) {
            stateHandler->createProjMenu->Render();
        }

        if(stateHandler->openProjMenu && stateHandler->openProjMenu->isMenuOpen) {
            stateHandler->openProjMenu->Render();
        }

        SDL_RenderPresent(rend);
    }

    if (stateHandler) {
        delete stateHandler->createProjMenu;
        delete stateHandler;
    }

    TTF_CloseFont(appFont);
    TTF_Quit();
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}