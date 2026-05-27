#ifndef __GLOBALSTATEHANDLER_H__
#define __GLOBALSTATEHANDLER_H__

namespace Menu { class CreateProjMenu; class OpenProjMenu; class ImageMenu;}

typedef struct globalStateHandler
{
    
    bool isCreateProjMenuOpen;
    Menu::CreateProjMenu* createProjMenu;

    Menu::OpenProjMenu* openProjMenu;
    Menu::ImageMenu* imageMenu;

    std::string currentProjectPath;

} globalStateHandler;


#endif