#ifndef __MONOPOLY__client__
#define __MONOPOLY__client__

#include "gameModule.h"
#include "gameplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 100

extern gamePacket gP;

extern int state;

extern bool diceState;

extern int currentPlayer;

extern int networkState;

extern int playerCount;

void *get_in_addr(struct sockaddr *);

void *client(void *);

#endif
