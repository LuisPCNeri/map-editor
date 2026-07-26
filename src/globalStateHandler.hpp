#ifndef __GLOBALSTATEHANDLER_H__
#define __GLOBALSTATEHANDLER_H__

namespace Menu { class CreateProjMenu; class OpenProjMenu; class ImageMenu; class SpawnIdMenu;}
namespace Map  { class MapRenderer; class MapViewport;}

#include <string>

enum class EditorMode {TILE_PAINT = 0, TRAINER_PLACE = 1};

typedef struct globalStateHandler
{
    
    bool isCreateProjMenuOpen;
    Menu::CreateProjMenu* createProjMenu;

    Menu::OpenProjMenu* openProjMenu;
    Menu::ImageMenu* imageMenu;

    Menu::SpawnIdMenu* spawnIdMenu;

    Map::MapRenderer* mapRenderer;
    Map::MapViewport* mapViewport;

    std::string currentProjectPath;
    EditorMode active_mode = EditorMode::TILE_PAINT;

} globalStateHandler;


#endif
