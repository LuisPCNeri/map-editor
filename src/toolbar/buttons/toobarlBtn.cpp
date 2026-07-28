#include "../toolbar.hpp"
#include "toolbarBtn.hpp"
#include "../../globalStateHandler.hpp"

#include <SDL2/SDL_surface.h>
#include <iostream>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

extern globalStateHandler* stateHandler;
extern SDL_Renderer* rend;

#define BASE_PROJ_FOLDER ".MapMakerProjects"

void SaveBtnHandleClick() {

    if(stateHandler->currentProjectPath.empty()) {
        std::cerr << "MUST HAVE A PROJECT OPEN TO SAVE." << std::endl;
        return; 
    }

    stateHandler->mapRenderer->ExportMapToBin(stateHandler->currentProjectPath + "/data/map.bin");
}

void HandleChooseProjectToOpen(std::string path) {
    
    /// To prevent serious problems here
    stateHandler->active_mode = EditorMode::TILE_PAINT;

    /// Previous project cleanup
    stateHandler->imageMenu->images.clear();
    stateHandler->imageMenu->importedImages.clear();

    stateHandler->trainer_menu->images.clear();
    stateHandler->trainer_menu->importedImages.clear();

    for (auto* surf : stateHandler->imageMenu->raw_surfaces) {
        if (surf) SDL_FreeSurface(surf);
    }
    stateHandler->imageMenu->raw_surfaces.clear();
    
    for(auto& surf : stateHandler->trainer_menu->raw_surfaces) {
        if(surf) SDL_FreeSurface(surf);
    }
    stateHandler->trainer_menu->raw_surfaces.clear();

    stateHandler->mapRenderer->grid.clear();

    stateHandler->currentProjectPath = path;
    
    /// Switching the active_mode ensures both manifests are loaded
    stateHandler->imageMenu->LoadFromManifest(path, rend);
    stateHandler->active_mode = EditorMode::TRAINER_PLACE;
    stateHandler->trainer_menu->LoadFromManifest(path, rend);
    stateHandler->active_mode = EditorMode::TILE_PAINT;

    stateHandler->mapRenderer->ImportMapFromBinary(path + "/data/map.bin");

    if(stateHandler->openProjMenu && stateHandler->openProjMenu->isMenuOpen) {
        stateHandler->openProjMenu->CloseMenu();
    }
}

Menu::OpenProjMenu* OpenBtnHandleClick() {
    if(stateHandler && stateHandler->openProjMenu) {
        stateHandler->openProjMenu->OpenMenu();
        return stateHandler->openProjMenu;
    }

    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        std::cerr << "CRITICAL ERROR: Could not find HOME environment variable!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string baseFolder = std::string(homeDir) + "/" + BASE_PROJ_FOLDER;
    struct stat info;
    if( stat(baseFolder.c_str(), &info) != 0 ) {
        std::cerr << "Cannot access: " << baseFolder << std::endl;

        mkdir(baseFolder.c_str(), 0777);
    }
    else if( info.st_mode & S_IFDIR ) {
        std::cout << "Directory is a folder." << std::endl;
    }

    int32_t w, h;
    SDL_GetRendererOutputSize(rend, &w, &h);

    vw_size_t width(20.0f);
    vh_size_t height(50.0f);

    int32_t x = (w/2) - (width/2);
    int32_t y = (h/2) - (height/2);

    Menu::OpenProjMenu* openProjMenu = new Menu::OpenProjMenu(x, y, 50.0f, 20.0f);
    std::vector<std::string> projects = Menu::OpenProjMenu::GetExistingProjects();

    for(const auto& project : projects) {
        Menu::MenuBtn btn(0, 0, 5.0f, 2.0f, project, [project]() {
            HandleChooseProjectToOpen(project);
        });

        openProjMenu->AddBtn(std::move(btn));
    }

    return openProjMenu;

}

void CreateProjMenuReturnClickHandle() {
    if (stateHandler && stateHandler->createProjMenu) {
        stateHandler->createProjMenu->text.clear();
        stateHandler->createProjMenu->Close();
    }
}

void CreateProjMenuCreateHandleClick() {
    std::cout << "CLICKED CREATE PROJECT" << std::endl;

    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        std::cerr << "CRITICAL ERROR: Could not find HOME environment variable!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string baseFolder = std::string(homeDir) + "/" + BASE_PROJ_FOLDER;
    struct stat info;
    if( stat(baseFolder.c_str(), &info) != 0 ) {
        std::cerr << "Cannot access: " << baseFolder << std::endl;

        mkdir(baseFolder.c_str(), 0777);
    }
    else if( info.st_mode & S_IFDIR ) {
        std::cout << "Directory is a folder." << std::endl;
    }

    std::string path = std::string(homeDir) + "/" + BASE_PROJ_FOLDER + "/" + stateHandler->createProjMenu->text;

    if(mkdir(path.c_str(), 0777) == -1) {
        std::cerr << "FAILED CREATE PROJ DIRECTORY!" << std::endl;
        exit(EXIT_FAILURE);
    };
    
    std::string fileIdentifierPath = std::string(homeDir) + "/" + BASE_PROJ_FOLDER + "/" + stateHandler->createProjMenu->text + "/.MapProject";

    FILE* file = fopen(fileIdentifierPath.c_str(), "w");
    if(!file) {
        std::cerr << "FAILED TO OPEN FILE!" << std::endl;
        exit(EXIT_FAILURE);
    }

    fprintf(file, "This is indeed a file.\n");
    fclose(file);

    std::string dataDir = path + "/data";
    if(mkdir(dataDir.c_str(), 0777)) {
        std::cerr << "FAILED CREATE DATA DIRECTORY!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string assetsDir = path + "/assets";
    if(mkdir(assetsDir.c_str(), 0777)) {
        std::cerr << "FAILED CREATE ASSETS DIRECTORY!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (stateHandler) {
        stateHandler->currentProjectPath = path;
    }

    if (stateHandler && stateHandler->createProjMenu) {
        stateHandler->createProjMenu->text.clear();
        stateHandler->createProjMenu->Close();
    }
}

Menu::CreateProjMenu* CreateProjHandleClick() {
    if (stateHandler && stateHandler->createProjMenu) {
        stateHandler->createProjMenu->Show();
        return stateHandler->createProjMenu;
    }

    int32_t w, h;
    SDL_GetRendererOutputSize(rend, &w, &h);

    vw_size_t width(30.0f);
    vh_size_t height(10.0f);

    int32_t x = (w/2) - (width/2);
    int32_t y = (h/2) - height;

    Menu::CreateProjMenu* createProjMenu = new Menu::CreateProjMenu(x, y, 30.0f, 10.0f);

    int32_t btn_x = x + (int32_t)width - vw_size_t(5.0f) * 2 - 10;
    int32_t btn_y = y + (int32_t) (vh_size_t(10.0f) - vh_size_t(2.0f) - (vh_size_t(10.0f) * .1f));

    Menu::MenuBtn createBtn(btn_x, btn_y, 5.0f, 2.0f, "Create", CreateProjMenuCreateHandleClick);

    Menu::MenuBtn returnBtn(btn_x, btn_y, 5.0f, 2.0f, "Return", CreateProjMenuReturnClickHandle);

    createProjMenu->AddBtn(std::move(createBtn));
    createProjMenu->AddBtn(std::move(returnBtn));

    return createProjMenu;
}
