#ifndef __MONOPOLY__server__
#define __MONOPOLY__server__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sstream>
#include "gameModule.h"
#include "gameplay.h"

#define BACKLOG 8

extern int state;

extern int networkState;

extern int currentPlayer;

extern gamePacket gP;

void sigchld_handler(int);

void *get_in_addr(struct sockaddr *);

void *server(void *);

#endif
