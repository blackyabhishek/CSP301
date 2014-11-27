#ifndef __LOParser__gameModule__
#define __LOParser__gameModule__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>
#ifdef __APPLE__
#  include <GLUI/glui.h>
#else
#  include <GL/glui.h>
#endif

extern GLUI_Listbox *listBox1,*listBox2,*listBox3,*listBox4,*listBox5,*listBox6;
extern GLUI_Listbox *listBoxM7,*listBoxM9,*listBoxM1,*listBoxM2,*listBoxM3,*listBoxM4,*listBoxM5,*listBoxM8;
extern bool first;
extern int taxType;
extern int pS1;
extern int pS2;
extern int pS3;
extern float amount1;
extern float amount2;
extern float amount3;
extern bool waitDice;
extern int currentPlayer;
extern int state;
class Game;
extern int rdm1,rdm2;
extern float d;
extern float rx1,ry1,rz1;
extern float rx2,ry2,rz2;
extern Game* M1;
extern Game* M;
struct gamePacket;
extern gamePacket gP;
struct Player;
extern char *cnum;
extern float h;
char* fta(float);
char* ita(int);
struct gamePacket
{
    short NS;
    short playerID;
    short locationID;
    bool buyCard;
    bool buyHouse;
    bool buyHotel;
    bool payRent;
    bool payJailFine;
    bool payTax;
    bool taxOption;
    bool buyBack;
    float amount1;
    float amount2;
    float amount3;
    short totalCheating;
    short cheating[8];
    short totalmortgage;
    short mortgageList[8];
};
void initializeGamePacket();
void getGamePacket(gamePacket*);
void packetPlay();
struct cityData
{
    int special;
    int locationID;
    int groupID;
    char *cityName;
    char *objName;
    int cityCost;
    int mortgageValue;
    int rent[6];
    int houseCost[5];
    int status;
    int statusCost;
    float lx;
    float lz;
    float ori;
    Player *owner;
    cityData()
    {
        special=0;
        owner=NULL;
        objName=NULL;
        cityName=NULL;
        status=0;
        statusCost=0;
    }
};
extern std::vector<cityData*> cityListMenu;
extern float x;
extern float z;
struct Player
{
    char *IP;
    char *playerName;
    int playerID;
    float money;
    float cheatAmount;
    int group[12];
    std::vector<cityData*> cards;
    cityData* currentPosition;
    Player()
    {
        cheatAmount=0.0;
        for(int i=0;i<12;i++)
            group[i]=0;
    }
};
class Game
{
public:
    char *serverIP;
    bool graph[32][32];
    int totalHumanPlayers;
    int totalBotPlayers;
    int totalPlayersLeft;
    char *currency;
    int startingMoney;
    int jailFine;
    int taxPercent;
    int taxAlternate;
    int totalCities;
    Player *homePlayer;
    std::vector<cityData*> cityList;
    std::vector<Player*> playerList;
    Player *getPointerByID(int);
    cityData *getPointerByLID(int);
    void listAllPossibleLocations(int,int);
    int buyCard(cityData*,Player*,float);
    int mortgageCard(cityData*,Player*);
    int buyMortgaged(cityData*,Player*);
    int payTax(Player*,int,float);
    int payJailFine(Player*,float);
    int buildHouse(cityData*,Player*,float);
    int buildHotel(cityData*,Player*,float);
    int payRent(cityData*,Player*);
    void throwDice(int,int);
    int reportCheating(Player*,Player*);
    void move(Player*,cityData*);
    void removePlayer(Player*);
};
void callBack(int id);
#endif
