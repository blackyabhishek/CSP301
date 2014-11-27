#include "gameplay.h"
//The function give turns to different players.
//networkstate 1 : server assigns player id to human players
//networkstate 2 : clients send thier names to the server
//networkstate 3 : server sends game data to all clients
//networkstate 4 : client sends dice data to server
//networkstate 5 : server sends dice data to all clients
//networkstate 6 : client sends turn data to server
//networkstate 7 : server sends turn data to all clients
void nextNetworkState()
{
    std::cout<<"####\n"<<currentPlayer<<" "<<networkState<<"\n";
    int id=M->homePlayer->playerID;
    if((networkState==6 && id==currentPlayer && id!=1) || networkState==7)
        while(1)
        {
            currentPlayer++;
            if(M->getPointerByID(currentPlayer)!=NULL)
                break;
            if(currentPlayer>M->totalHumanPlayers)
                currentPlayer=-M->totalBotPlayers-1;
        }
    if(id==currentPlayer && state==3)
        state=0;
    if(currentPlayer<0 && networkState==5 && id==1)
    {
        botPlayer bP;
        bP.botPlay(M->getPointerByID(currentPlayer));
    }
    if(networkState==3)
    {
        if(currentPlayer<=1)
            networkState=5;
        else if(id==1 || id==currentPlayer)
            networkState=4;
        else
            networkState=5;
    }
    else if(networkState==4)
    {
        if(id==1 && currentPlayer<=1)
            networkState=5;
        else
            networkState=6;
    }
    else if(networkState==5)
    {
        while(d!=-100.0);
        if(currentPlayer<=1)
            networkState=7;
        else if(id==1)
            networkState=6;
        else
            networkState=7;
    }
    else if(networkState==6)
    {
        if(id==1)
            networkState=7;
        else
            networkState=5;
    }
    else if(networkState==7)
    {
        if(currentPlayer>1 && (id==1 || id==currentPlayer))
            networkState=4;
        else
            networkState=5;
    }
    std::cout<<currentPlayer<<" "<<networkState<<"\n********\n";
}
