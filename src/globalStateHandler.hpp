#ifndef __GLOBALSTATEHANDLER_H__
#define __GLOBALSTATEHANDLER_H__

namespace Menu { class CreateProjMenu; class OpenProjMenu; class ImageMenu;}
namespace Map  {class MapRenderer;}

typedef struct globalStateHandler
{
    
    bool isCreateProjMenuOpen;
    Menu::CreateProjMenu* createProjMenu;

    Menu::OpenProjMenu* openProjMenu;
    Menu::ImageMenu* imageMenu;

    Map::MapRenderer* mapRenderer;

    std::string currentProjectPath;

} globalStateHandler;


#endif