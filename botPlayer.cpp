#include "botPlayer.h"
//The function tells the bot which city to buy from a given set of choice(s).
cityData *botPlayer::buy()
{
    cityData *cD1=NULL,*cD2=NULL,*cD3=NULL;
    
    int n1=10,n2=10;
    for(int i=0,n=0,m=0;i<locations.size();i++)
    {
        n=p->group[locations[i]->groupID];
        m=locations[i]->statusCost;
        if(locations[i]->owner==NULL && locations[i]->cityCost<0.5*p->money)
        {
            if(n1!=2 && n==2)
            {
                n1=2;
                cD1=locations[i];
                type=1;
            }
            else if(n1!=1 && n==1)
            {
                n1=1;
                cD1=locations[i];
                type=1;
            }
            else if(n1<3 && n>=3)
            {
                n1=n;
                cD1=locations[i];
                type=1;
            }
            else
            {
                cD1=locations[i];
                type=1;
            }
        }
        else if(locations[i]->owner==p && n>=3 && locations[i]->houseCost[m]<0.2*p->money)
        {
            if(n2>m && m<4)
            {
                n2=m;
                cD2=locations[i];
                type=2;
            }
            else if(n2>m && m==4)
            {
                n2=m;
                cD2=locations[i];
                type=3;
            }
        }
        else if(locations[i]->owner==p)
        {
            cD3=locations[i];
            type=4;
        }
    }
    if(cD1!=NULL)
        return cD1;
    else if(cD2!=NULL)
        return cD2;
    return cD3;
}
//The function mortgages the city for a bot.
cityData *botPlayer::mortgage()
{
    cityData *cD=NULL;
    float m=0.0;
    for(int i=0;i<p->cards.size();i++)
    {
        if(m<p->cards[i]->mortgageValue)
        {
            cD=p->cards[i];
            m=p->cards[i]->mortgageValue;
        }
    }
    return cD;
}
//The function tells the bot which city's rent to pay
cityData *botPlayer::pay()
{
    cityData *cD=NULL;
    float m=1E100;
    for(int i=0;i<locations.size();i++)
    {
        if(locations[i]->owner!=p && locations[i]->owner!=NULL && locations[i]->special==0)
        {
            if(m>locations[i]->rent[locations[i]->statusCost])
            {
                m=locations[i]->rent[locations[i]->statusCost];
                cD=locations[i];
                type=5;
            }
        }
        else if(locations[i]->special==1) // Tax
        {
            float tax=(((M->taxPercent*p->money)/100)<M->taxAlternate)?((M->taxPercent*p->money)/100):M->taxAlternate;
            taxType=(((M->taxPercent*p->money)/100)<M->taxAlternate)?0:1;
            if(m>tax)
            {
                m=tax;
                cD=locations[i];
                type=6;
            }
        }
        else if(locations[i]->special==2) // Jail Fine
        {
            if(m>M->jailFine)
            {
                m=M->jailFine;
                cD=locations[i];
                type=7;
            }
        }
    }
    while(p->money<m && p->cards.size()>0)
        M->mortgageCard(mortgage(),p);
    return cD;
}
void botPlayer::botPlay(Player *p1)
{
    type=-1;
    taxType=-1;
    
    p=p1;
    
    cityData *cD;
    std::cout<<"WAITING "<<p->playerName<<"\n";
    while(d!=-100.0);
    
    if(M->homePlayer->playerID==1)
        M->throwDice(-1,-1);
    else
        M->throwDice(rdm1,rdm2);
    
    while(d!=-100.0);
    
    M->listAllPossibleLocations(p->currentPosition->locationID, rdm1+rdm2+2);
    
    for(int i=0;i<cityListMenu.size();i++)
        locations.push_back(cityListMenu[i]);
    
    cD=buy();
    
    gP.playerID=p->playerID;
    
    if(cD!=NULL)
    {
        M->move(p,cD);
        if(type==1)
        {
            M->buyCard(cD,p,cD->cityCost);
            gP.buyCard=true;
        }
        else if(type==2)
        {
            M->buildHouse(cD,p,cD->houseCost[cD->statusCost]);
            gP.buyHouse=true;
        }
        else if(type==3)
        {
            M->buildHotel(cD,p,cD->houseCost[cD->statusCost]);
            gP.buyHotel=true;
        }
        else if(type==4)
        {
            //IDLE
        }
        p->currentPosition=cD;
    }
    else
    {
        cD=pay();
        if(type==5)
        {
            M->payRent(cD,p);
            gP.payRent=true;
        }
        else if (type==6)
        {
            M->payTax(p,taxType,(taxType==0)?((M->taxPercent*p->money)/100):M->taxAlternate);
            gP.payTax=true;
            gP.taxOption=(taxType==0)?true:false;
        }
        else if(type==7)
        {
            M->payJailFine(p,M->jailFine);
            gP.payJailFine=true;
        }
        p->currentPosition=cD;
    }
    locations.erase(locations.begin(),locations.end());
    p=NULL;
    char *cnum=new char[2];
    cnum=new char[10];
    std::stringstream ss;
    ss<<M->homePlayer->money;
    ss>>cnum;
    callBack(0);
}
