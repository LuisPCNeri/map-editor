#include "../toolbar.hpp"
#include "toolbarBtn.hpp"
#include "../../globalStateHandler.hpp"

#include <iostream>

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
    /// IMPORTANT missing real handling for creating the project directory and shit
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

    Menu::MenuBtn createBtn(btn_x, btn_y, 5.0f, 2.0f, "Create", CreateProjHandleClick);

    Menu::MenuBtn returnBtn(btn_x, btn_y, 5.0f, 2.0f, "Return", CreateProjMenuReturnClickHandle);

    createProjMenu->AddBtn(std::move(createBtn));
    createProjMenu->AddBtn(std::move(returnBtn));

    return createProjMenu;
}