#ifndef __TOOLBAR_BTN_H__
#define __TOOLBAR_BTN_H__

#include "../toolbar.hpp"
#include "../../toolbarMenus/createProjMenu.hpp"
#include "../../toolbarMenus/openProjMenu.hpp"

void SaveBtnHandleClick();

Menu::OpenProjMenu* OpenBtnHandleClick();
Menu::CreateProjMenu* CreateProjHandleClick();

#endif