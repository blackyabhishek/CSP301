#ifndef __MONOPOLY__gameplay__
#define __MONOPOLY__gameplay__

#include <pthread.h>
#include <iostream>
#include "gameModule.h"
#include "botPlayer.h"
#include "server.h"
#include "client.h"

extern int currentPlayer;
extern int state;
extern Game* M;
extern bool diceState;
extern float d;

void nextNetworkState();

#endif
