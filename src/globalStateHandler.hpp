#ifndef __GLOBALSTATEHANDLER_H__
#define __GLOBALSTATEHANDLER_H__

namespace Menu { class CreateProjMenu; }

typedef struct globalStateHandler
{
    
    bool isCreateProjMenuOpen;
    Menu::CreateProjMenu* createProjMenu;

} globalStateHandler;


#endif