#include "../toolbar.hpp"
#include "toolbarBtn.hpp"
#include "../../globalStateHandler.hpp"

#include <iostream>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

extern globalStateHandler* stateHandler;
extern SDL_Renderer* rend;

void SaveBtnHandleClick() {

}

void OpenBtnHandleClick() {

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

    std::string path = std::string(homeDir) + "/" + stateHandler->createProjMenu->text;

    if(mkdir(path.c_str(), 0777) == -1) {
        std::cerr << "FAILED CREATE PROJ DIRECTORY!" << std::endl;
        exit(EXIT_FAILURE);
    };
    
    std::string fileIdentifierPath = std::string(homeDir) + "/" + stateHandler->createProjMenu->text + "/.MapProject";

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