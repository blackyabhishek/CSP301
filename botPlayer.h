#ifndef __LOParser__botPlayer__
#define __LOParser__botPlayer__

#include <stdio.h>
#include "gameModule.h"
#include <sstream>
#ifdef __APPLE__
#  include <GLUI/glui.h>
#else
#  include <GL/glui.h>
#endif

extern Game *M;
extern int rdm1,rdm2;
extern GLUI_Listbox *listBox1;

class botPlayer
{
    std::vector<cityData*> locations;
    Player *p;
    int type;
    int taxType;
public:
    cityData *buy();
    cityData *pay();
    cityData *mortgage();
    void botPlay(Player *);
};
#endif
