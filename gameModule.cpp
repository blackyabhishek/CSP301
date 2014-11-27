#include "gameModule.h"
//The function initializes game packet which is sent over thw network.
void initializeGamePacket()
{
    gP.playerID=0;
    gP.locationID=-1;
    gP.buyCard=false;
    gP.buyHouse=false;
    gP.buyHotel=false;
    gP.payRent=false;
    gP.payJailFine=false;
    gP.payTax=false;
    gP.taxOption=false;
    gP.buyBack=false;
    gP.totalCheating=0;
    gP.totalmortgage=0;
    gP.amount1=0.0;
    gP.amount2=0.0;
    gP.amount3=0.0;
    gP.NS=0;
    for(int i=0;i<8;i++)
    {
        gP.cheating[i]=0;
        gP.mortgageList[i]=-1;
    }
}
//The function assigns the received data fro network to a data structure
void getGamePacket(gamePacket *gP1)
{
    gP.playerID=gP1->playerID;
    gP.locationID=gP1->locationID;
    gP.buyCard=gP1->buyCard;
    gP.buyHouse=gP1->buyHouse;
    gP.buyHotel=gP1->buyHotel;
    gP.payRent=gP1->payRent;
    gP.payJailFine=gP1->payJailFine;
    gP.payTax=gP1->payTax;
    gP.taxOption=gP1->taxOption;
    gP.buyBack=gP1;
    gP.totalCheating=gP1->totalCheating;
    gP.totalmortgage=gP1->totalmortgage;
    gP.NS=gP1->NS;
    for(int i=0;i<8;i++)
    {
        gP.cheating[i]=gP1->cheating[i];
        gP.mortgageList[i]=gP1->mortgageList[i];
    }
}
//The function plays the game based on the value of structure gP which is initialized by getGamePacket.
void packetPlay()
{
    Player *p=M->getPointerByID(gP.playerID);
    cityData *cD=M->getPointerByLID(gP.locationID);
    if(p!=NULL && cD!=NULL)
    {
        p->currentPosition=cD;
        M->move(p,cD);
        for(int i=0;i<gP.totalmortgage;i++)
            if(M->getPointerByLID(gP.mortgageList[i])!=NULL)
                M->mortgageCard(M->getPointerByLID(gP.mortgageList[i]),p);
        if(gP.buyCard)
            M->buyCard(p->currentPosition,p,gP.amount1);
        if(gP.buyHouse)
            M->buildHouse(p->currentPosition,p,gP.amount1);
        if(gP.buyHotel)
            M->buildHotel(p->currentPosition,p,gP.amount1);
        if(gP.payRent)
            M->payRent(p->currentPosition,p);
        if(gP.payTax)
            M->payTax(p,(gP.taxOption)?0:1,gP.amount2);
        if(gP.payRent)
            M->payJailFine(p,gP.amount3);
        if(gP.buyBack)
            M->buyMortgaged(p->currentPosition,p);
        for(int i=0;i<gP.totalCheating;i++)
            if(M->getPointerByID(gP.cheating[i])!=NULL)
                M->reportCheating(M->getPointerByID(gP.cheating[i]),p);
    }
    else
    {
        std::cout<<"PLAYER NOT FOUND\n";
    }
}
//Converts integer to ascii
char* ita(int num)
{
    cnum=new char[10];
    std::stringstream ss;
    ss<<num;
    ss>>cnum;
    return cnum;
}
//Converts float to ascii
char* fta(float num)
{
    cnum=new char[32];
    std::stringstream ss;
    ss<<num;
    ss>>cnum;
    return cnum;
}
//The function links buttons to corresponding functions and variables.
void callBack(int id)
{
    std::cout<<"Callback : "<<id<<"\n";
    
    for(int i=-1;i<32;i++)
        listBox5->delete_item(i);
    cityListMenu.erase(cityListMenu.begin(),cityListMenu.end());
    M->listAllPossibleLocations(M->homePlayer->currentPosition->locationID,rdm1+rdm2+2);
    listBox5->add_item(-1,"Slide Down");
    for(int i=0;i<cityListMenu.size();i++)
    {
        if(cityListMenu[i]==NULL)
            break;
        listBox5->add_item(cityListMenu[i]->locationID,cityListMenu[i]->cityName);
    }
    
    Player *p=M->getPointerByID(pS1);
    Player *ph=M->homePlayer;
    cityData *cD=M->getPointerByLID(pS2);
    
    listBox6->delete_item(0);
    listBox6->add_item(0,M->getPointerByID(currentPlayer)->playerName);
    
    if(id==11 && state==4)
        if(pS3>=0)
        {
            std::cout<<""<<"\n";
            M->move(M->homePlayer, M->getPointerByLID(pS3));
            //p->currentPosition=M->getPointerByLID(pS3);
            gP.locationID=pS3;
            gP.playerID=M->homePlayer->playerID;
            state=1;
        }
    
    if(id==2 && (state==1 || state==2))
    {
        gP.cheating[gP.totalCheating++]=p->playerID;
        M->reportCheating(ph,p);
    }
    else if(id==4 && (state==1 || state==2))
    {
        if(M->buyCard(ph->currentPosition, ph,amount1))
        {
            gP.buyCard=true;
            state=3;
        }
    }
    else if(id==5 && (state==1 || state==2))
    {
        if(M->buildHouse(ph->currentPosition,ph,amount1))
        {
            gP.buyHouse=true;
            state=3;
        }
    }
    else if(id==6 && (state==1 || state==2))
    {
        if(M->buildHotel(ph->currentPosition,ph,amount1))
        {
            gP.buyHotel=true;
            state=3;
        }
    }
    else if(id==7 && (state==1 || state==2))
    {
        if(M->payRent(ph->currentPosition,ph))
        {
            gP.payRent=true;
            state=3;
        }
    }
    else if(id==8 && (state==1 || state==2))
    {
        if(M->payTax(ph,taxType,amount2))
        {
            gP.payTax=true;
            gP.taxOption=(taxType==0)?true:false;
            state=3;
        }
    }
    else if(id==9 && (state==1 || state==2))
    {
        if(M->payJailFine(ph,amount3))
        {
            gP.payJailFine=true;
            state=3;
        }
    }
    else if(id==10 && (state==1 || state==2))
    {
        if(M->mortgageCard(cD,ph))
        {
            gP.mortgageList[gP.totalmortgage++]=cD->locationID;
            state=2;
        }
        else
        {
            gP.buyBack=true;
            M->buyMortgaged(cD,p);
            state=3;
        }
    }
    
    {
        listBox1->delete_item(0);
        listBox1->add_item(0,ita(ph->money));
    }
    
    {
        for(int i=0;i<M->totalCities;i++)
        {
            listBox3->delete_item(i);
            listBoxM9->delete_item(i);
        }
        
        for(int i=0;i<p->cards.size();i++)
            listBox3->add_item(p->cards[i]->locationID,p->cards[i]->cityName);
        if(p->cards.size()==0)
            listBox3->add_item(0," ");
        if(p->currentPosition!=NULL)
            listBoxM9->add_item(p->currentPosition->locationID,p->currentPosition->cityName);
    }
    
    {
        listBoxM1->delete_item(0);
        listBoxM2->delete_item(0);
        listBoxM4->delete_item(0);
        for(int i=0;i<6;i++)
        {
            listBoxM3->delete_item(i);
            listBoxM8->delete_item(i);
        }
        
        if(cD->statusCost==0)
            listBoxM1->add_item(0,"Basic");
        else if(cD->statusCost==1)
            listBoxM1->add_item(0,"House1");
        else if(cD->statusCost==2)
            listBoxM1->add_item(0,"House2");
        else if(cD->statusCost==3)
            listBoxM1->add_item(0,"House3");
        else if(cD->statusCost==4)
            listBoxM1->add_item(0,"House4");
        else if(cD->statusCost==5)
            listBoxM1->add_item(0,"Hotel");
        if(cD->owner!=NULL)
            listBoxM2->add_item(0,cD->owner->playerName);
        else
            listBoxM2->add_item(0," ");
        for(int i=0;i<6;i++)
        {
            if(i==0)
            {
                listBoxM3->add_item(i,ita(cD->cityCost));
                delete cnum;
            }
            else
            {
                listBoxM3->add_item(i,ita(cD->houseCost[i-1]));
                delete cnum;
            }
            listBoxM8->add_item(i,ita(cD->rent[i]));
            delete cnum;
        }
        listBoxM4->add_item(0,ita(cD->mortgageValue));
        delete cnum;
    }
    
    {
        listBoxM5->delete_item(0);
        listBoxM5->add_item(0,M->homePlayer->currentPosition->cityName);
    }
    first=false;
    
}
//Function to get pointer to a location by player ID
Player* Game::getPointerByID(int id)
{
    Player *p=NULL;
    std::cout<<"";
    for(int i=0;i<playerList.size();i++)
    {
        if(playerList[i]->playerID==id)
        {
            p=playerList[i];
            break;
        }
    }
    return p;
}
//Function to get pointer to a location by location ID
cityData* Game::getPointerByLID(int id)
{
    cityData *cD=NULL;
    for(int i=0;i<cityList.size();i++)
    {
        if(cityList[i]->locationID==id)
        {
            cD=cityList[i];
            break;
        }
    }
    return cD;
}
//Function to list all possible combinations that a player could go from a point
void Game::listAllPossibleLocations(int lID,int num)
{
    //std::cout<<"C : "<<num<<" "<<lID<<" "<<getPointerByLID(lID)->cityName<<"\n";
    if(num==0)
    {
        cityListMenu.push_back(getPointerByLID(lID));
    }
    else
    {
        for(int i=0;i<32;i++)
        {
            if(graph[lID][i]==true)
                listAllPossibleLocations(i,num-1);
        }
    }
}
//Function to pay rent
int Game::payRent(cityData *cD,Player *p)
{
    listBoxM7->delete_item(0);
    if(cD->owner!=p)
    {
        if(cD->status==10)
        {
            float rent=cD->rent[cD->statusCost];
            if(p->money>rent)
            {
                listBoxM7->add_item(0,"Rent Paid");
                p->money-=rent;
                cD->owner->money+=rent;
                return 1;
            }
            else
            {
                if(p->cards.size()==0)
                {
                    std::cout<<"Game Over "<<p->playerName<<" !\n";
                    removePlayer(p);
                }
                else
                {
                    listBoxM7->add_item(0,"Mortgage a card to continue");
                }
            }
        }
    }
    return 0;
}
//Function to remove lost player
void Game::removePlayer(Player* p)
{
    int pos=0;
    listBoxM7->add_item(0,"A player lost");
    for(int i=0;i<playerList.size();i++)
        if(playerList[i]==p)
        {
            pos=i;
            break;
        }
    playerList.erase(playerList.begin()+pos);
}
//Function to buy card
int Game::buyCard(cityData *cD,Player *p,float amount)
{
    listBoxM7->delete_item(0);
    if(cD->owner==NULL && cD->special==0)
    {
        if(p->money>=amount)
        {
            listBoxM7->add_item(0,"City Purchased");
            cD->owner=p;
            cD->status=10;
            p->money-=amount;
            p->cards.push_back(cD);
            p->group[cD->groupID]++;
            //if(cD->cityCost>amount)
                p->cheatAmount=cD->cityCost-amount;
            return 1;
        }
        else
            listBoxM7->add_item(0,"Insufficient amount");
    }
    else
        listBoxM7->add_item(0,"Card sold out");
    return 0;
}
//Function to mortgage a card
int Game::mortgageCard(cityData *cD,Player *p)
{
    listBoxM7->delete_item(0);
    if(cD->owner==p)
    {
        if(cD->status==10)
        {
            listBoxM7->add_item(0,"Mortgage Successful");
            cD->status=5;
            p->money+=cD->mortgageValue;
            p->group[cD->groupID]--;
            return 1;
        }
        else
            listBoxM7->add_item(0,"Already Mortgaged");
    }
    else
        listBoxM7->add_item(0,"Card not owned by you");
    return 0;
}
//Function to buy back the mortgaged card
int Game::buyMortgaged(cityData *cD,Player *p)
{
    listBoxM7->delete_item(0);
    if(cD->owner==p)
    {
        if(cD->status==5)
        {
            if(p->money>=cD->mortgageValue)
            {
                if(p->currentPosition==cD)
                {
                    listBoxM7->add_item(0,"City bought back");
                    p->money-=cD->mortgageValue;
                    cD->status=10;
                    p->group[cD->groupID]++;
                    return 1;
                }
                listBoxM7->add_item(0,"Not on that city");
            }
            else
                listBoxM7->add_item(0,"Insufficient amount");
        }
        else
            listBoxM7->add_item(0,"Card not mortgaged");
    }
    else
        listBoxM7->add_item(0,"Card not owned by you");
    return 0;
}
//Function to pay tax.
int Game::payTax(Player *p,int type,float amount)
{
    listBoxM7->delete_item(0);
    float tax=(type==0)?((taxPercent*p->money)/100):taxAlternate;
    if(p->money>=amount)
    {
        listBoxM7->add_item(0,"Tax paid");
        p->money-=amount;
        //if(tax>amount)
            p->cheatAmount=tax-amount;
        return 1;
    }
    else
    {
        if(p->cards.size()==0)
        {
            std::cout<<"Game Over "<<p->playerName<<" !\n";
            M->totalPlayersLeft--;
            removePlayer(p);
        }
        else
        {
            listBoxM7->add_item(0,"Mortgage to continue");
        }
    }
    return 0;
}
//Function to pay jail fine.
int Game::payJailFine(Player *p,float amount)
{
    listBoxM7->delete_item(0);
    if(p->money>=jailFine)
    {
        listBoxM7->add_item(0,"Jail fine paid");
        p->money-=jailFine;
        return 1;
    }
    else
    {
        if(p->cards.size()==0)
        {
            std::cout<<"Game Over "<<p->playerName<<" !\n";
            M->totalPlayersLeft--;
            removePlayer(p);
        }
        else
        {
            listBoxM7->add_item(0,"Mortgage to continue");
        }
    }
    return 0;
}
//Function to build house
int Game::buildHouse(cityData *cD, Player *p,float amount)
{
    listBoxM7->delete_item(0);
    if(cD->owner==p)
    {
        if(cD->status==10)
        {
            if(cD->statusCost<5)
            {
                if(p->money>=amount)
                {
                    listBoxM7->add_item(0,"House Built");
                    p->money-=amount;
                    float hC=cD->houseCost[cD->statusCost];
                    cD->statusCost++;
                    //if(hC>amount)
                        p->cheatAmount=hC-amount;
                    return 1;
                }
                else
                    listBoxM7->add_item(0,"Insufficient Amount");
            }
            else
                listBoxM7->add_item(0,"Have 4 houses");
        }
        else
            listBoxM7->add_item(0,"Ticekt is mortgaged");
    }
    else
        listBoxM7->add_item(0,"Ticket not owned by you");
    return 0;
}
//Function to build hotel
int Game::buildHotel(cityData *cD, Player *p,float amount)
{
    listBoxM7->delete_item(0);
    if(cD->owner==p)
    {
        if(cD->status==10)
        {
            if(cD->statusCost==4)
            {
                if(p->money>=amount)
                {
                    listBoxM7->add_item(0,"Hotel Built");
                    p->money-=amount;
                    float hC=cD->houseCost[cD->statusCost];
                    cD->statusCost++;
                    //if(hC>amount)
                        p->cheatAmount=hC-amount;
                    return 1;
                }
                else
                    listBoxM7->add_item(0,"Insufficient Amount");
            }
            else if(cD->statusCost==5)
                listBoxM7->add_item(0,"Already have a hotel");
            else
                listBoxM7->add_item(0,"Require 4 houses");
        }
        else
            listBoxM7->add_item(0,"Ticekt is mortgaged");
    }
    else
        listBoxM7->add_item(0,"Ticket not owned by you");
    return 0;
}
//Function to report cheating
int Game::reportCheating(Player *pr,Player *pc)
{
    listBoxM7->delete_item(0);
    if(pr==pc)
    {
        listBoxM7->add_item(0,"No self cheating report");
        return 0;
    }
    if(pc->cheatAmount>0)
    {
        pr->money+=0.1*pc->cheatAmount;
        if(pc->money>=pc->cheatAmount)
        {
            listBoxM7->add_item(0,"Successful Report");
            pc->money-=pc->cheatAmount;
            pc->cheatAmount=0.0;
        }
        else
        {
            if(pc->cards.size()==0)
            {
                std::cout<<"Game Over "<<pc->playerName<<" !\n";
                M->totalPlayersLeft--;
                removePlayer(pc);
            }
            else
            {
                listBoxM7->add_item(0,"Mortgage a card");
            }
        }
        return 1;
    }
    else
    {
        listBoxM7->add_item(0,"Unsuccessful Report");
        pr->money*=0.9;
    }
    return 0;
}
//Function to move a player
void Game::move(Player *p,cityData *cD)
{
    p->currentPosition=cD;
}
//Function to throw dice
void Game::throwDice(int r1=-1,int r2=-1)
{
    if(d==-100.0)
    {
        d=0.0;
        h=0.0;
        if(r1<0)
        {
            srand (time(NULL));
            rdm1=rand()%6;
            rdm2=rand()%6;
        }
        else
        {
            rdm1=r1;
            rdm2=r2;
        }
        //std::cout<<rdm1<<"\n";
        if(rdm1==0)
        {
            rx1=0;
            ry1=0;
            rz1=180;
        }
        else if(rdm1==1)
        {
            rx1=-90;
            ry1=0;
            rz1=0;
        }
        else if(rdm1==2)
        {
            rx1=0;
            ry1=0;
            rz1=-90;
        }
        else if(rdm1==3)
        {
            rx1=0;
            ry1=0;
            rz1=90;
        }
        else if(rdm1==4)
        {
            rx1=90;
            ry1=0;
            rz1=0;
        }
        else if(rdm1==5)
        {
            rx1=0;
            ry1=0;
            rz1=0;
        }
        if(rdm2==0)
        {
            rx2=0;
            ry2=0;
            rz2=180;
        }
        else if(rdm2==1)
        {
            rx2=-90;
            ry2=0;
            rz2=0;
        }
        else if(rdm2==2)
        {
            rx2=0;
            ry2=0;
            rz2=-90;
        }
        else if(rdm2==3)
        {
            rx2=0;
            ry2=0;
            rz2=90;
        }
        else if(rdm2==4)
        {
            rx2=90;
            ry2=0;
            rz2=0;
        }
        else if(rdm2==5)
        {
            rx2=0;
            ry2=0;
            rz2=0;
        }
        if(state==0)
        {
            callBack(0);
            state=4;
        }
    }
}
