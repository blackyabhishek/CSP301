#include "client.h"
//The structure sent during the networkstate 3. It contains parsed data.
struct packet1
{
    bool graph[32][32];
    int totalHumanPlayers;
    int totalBotPlayers;
    char currency[16];
    int startingMoney;
    int jailFine;
    int taxPercent;
    int taxAlternate;
    int totalCities;
};
//The structure sent during the networkstate 3. It contains city data.
struct packet2
{
    int special;
    int locationID;
    int groupID;
    char cityName[32];
    char objName[128];
    int cityCost;
    int mortgageValue;
    int rent[6];
    int houseCost[5];
    float x;
    float z;
    float o;
};
//The structure sent during the networkstate 3. It contains player data.
struct packet3
{
    char IP[16];
    char playerName[32];
    int playerID;
};
struct rdm
{
    short NS;
    int r1;
    int r2;
};
//The function gets ip address
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//The function is used by both clients and server.
//Server receives data from client when networkstate is 2 or 4 or 6
//Client receives data from server when networkstate is 1 or 3 or 5 or 7
void *client(void *n)
{
    int hPID=M->homePlayer->playerID;
    const char *PORT="3490";
    if(hPID==1)
        PORT="3491";
    else
        PORT="3490";
    while(1)
    {
        if(networkState==1 && hPID!=1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            char *argv=M->serverIP;
            bool flag=true;
            char *ch = NULL;
            while (flag)
            {
                int sockfd = 0, numbytes;
                char buf[MAXDATASIZE];
                struct addrinfo hints, *servinfo, *p;
                int rv;
                char s[INET6_ADDRSTRLEN];
                
                memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                    continue;
                    //return 1;
                }
                
                for(p = servinfo; p != NULL; p = p->ai_next)
                {
                    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                    {
                        perror("client: socket");
                        continue;
                    }
                    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                    {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                    }
                    break;
                }
                if (p == NULL)
                {
                    fprintf(stderr, "client: failed to connect\n");
                    continue;
                    //return 2;
                }
                inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                printf("client: connecting to %s\n", s);
                freeaddrinfo(servinfo);
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                if(sizeof(buf)>0)
                {
                    flag=false;
                    ch=new char[sizeof(buf)];
                    for(int i=0;i<sizeof(buf);i++)
                        ch[i]=buf[i];
                }
                printf("client: received '%s'\n",buf);
                close(sockfd);
            }
            hPID=atoi(ch);
            networkState=2;
        }
        else if(networkState==2 && hPID==1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            int pc=2;
            while(pc<=M->totalHumanPlayers)
            {
                
                char *argv=M->playerList[pc-1]->IP;
                
                int sockfd = 0, numbytes;
                char buf[MAXDATASIZE];
                struct addrinfo hints, *servinfo, *p;
                int rv;
                char s[INET6_ADDRSTRLEN];
                
                memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                    continue;
                    //return 1;
                }
                
                for(p = servinfo; p != NULL; p = p->ai_next)
                {
                    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                    {
                        perror("client: socket");
                        continue;
                    }
                    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                    {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                    }
                    break;
                }
                if (p == NULL)
                {
                    fprintf(stderr, "client: failed to connect\n");
                    continue;
                    //return 2;
                }
                inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                printf("client: connecting to %s\n", s);
                
                freeaddrinfo(servinfo);
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                if(sizeof(buf)>0)
                {
                    char *ch=new char[sizeof(buf)];
                    for(int i=0;i<sizeof(buf);i++)
                        ch[i]=buf[i];
                    M->playerList[pc-1]->playerName=ch;
                    pc++;
                }
                printf("client: received '%s'\n",buf);
                close(sockfd);
            }
            networkState=3;
        }
        else if(networkState==3 && hPID!=1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            while(networkState==3)
            {
                char *argv=M->serverIP;
                bool flag=true;
                while (flag)
                {
                    int sockfd = 0, numbytes;
                    char buf[1100];
                    struct addrinfo hints, *servinfo, *p;
                    int rv;
                    char s[INET6_ADDRSTRLEN];
                    
                    memset(&hints, 0, sizeof hints);
                    hints.ai_family = AF_UNSPEC;
                    hints.ai_socktype = SOCK_STREAM;
                    if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                    {
                        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                        continue;
                        //return 1;
                    }
                    
                    for(p = servinfo; p != NULL; p = p->ai_next)
                    {
                        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                        {
                            perror("client: socket");
                            continue;
                        }
                        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                        {
                            close(sockfd);
                            perror("client: connect");
                            continue;
                        }
                        break;
                    }
                    if (p == NULL)
                    {
                        fprintf(stderr, "client: failed to connect\n");
                        continue;
                        //return 2;
                    }
                    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                    printf("client: connecting to %s\n", s);
                    freeaddrinfo(servinfo);
                    if ((numbytes = recv(sockfd, buf, 1100-1, 0)) == -1)
                    {
                        perror("recv");
                        exit(1);
                    }
                    buf[numbytes] = '\0';
                    if(sizeof(buf)>0)
                    {
                        packet1 *p1=(packet1*)buf;
                        flag=false;
                        char sIP[16];
                        for(int j=0;M->serverIP[j]!='\0';j++)
                            sIP[j]=M->serverIP[j];
                        
                        delete M;
                        M=new Game;
                        M->currency=new char[16];
                        M->serverIP=new char[16];
                        
                        for(int i=0;i<32;i++)
                            for(int j=0;j<32;j++)
                                M->graph[i][j]=p1->graph[i][j];
                        M->jailFine=p1->jailFine;
                        M->startingMoney=p1->startingMoney;
                        M->taxAlternate=p1->taxAlternate;
                        M->taxPercent=p1->taxPercent;
                        M->totalBotPlayers=p1->totalBotPlayers;
                        M->totalCities=p1->totalCities;
                        M->totalHumanPlayers=p1->totalHumanPlayers;
                        M->totalPlayersLeft=M->totalBotPlayers+M->totalHumanPlayers;
                        for(int j=0;p1->currency[j]!='\0';j++)
                            M->currency[j]=p1->currency[j];
                        for(int j=0;sIP[j]!='\0';j++)
                            M->serverIP[j]=sIP[j];

                    }
                    printf("client: received '%s'\n",buf);
                    close(sockfd);
                }
                for(int i=0;i<M->totalCities;)
                {
                    int sockfd = 0, numbytes;
                    char buf[256];
                    struct addrinfo hints, *servinfo, *p;
                    int rv;
                    char s[INET6_ADDRSTRLEN];
                    
                    memset(&hints, 0, sizeof hints);
                    hints.ai_family = AF_UNSPEC;
                    hints.ai_socktype = SOCK_STREAM;
                    if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                    {
                        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                        continue;
                        //return 1;
                    }
                    
                    for(p = servinfo; p != NULL; p = p->ai_next)
                    {
                        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                        {
                            perror("client: socket");
                            continue;
                        }
                        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                        {
                            close(sockfd);
                            perror("client: connect");
                            continue;
                        }
                        break;
                    }
                    if (p == NULL)
                    {
                        fprintf(stderr, "client: failed to connect\n");
                        continue;
                        //return 2;
                    }
                    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                    printf("client: connecting to %s\n", s);
                    freeaddrinfo(servinfo);
                    if ((numbytes = recv(sockfd, buf, 256-1, 0)) == -1)
                    {
                        perror("recv");
                        exit(1);
                    }
                    buf[numbytes] = '\0';
                    if(sizeof(buf)>0)
                    {
                        i++;
                        packet2 *p2=(packet2*)buf;
                        
                        cityData *cD=new cityData;
                        
                        cD->cityName=new char[32];
                        cD->objName=new char[128];
                        
                        cD->special=p2->special;
                        cD->groupID=p2->groupID;
                        cD->locationID=p2->locationID;
                        cD->mortgageValue=p2->mortgageValue;
                        cD->cityCost=p2->cityCost;
                        for(int j=0;p2->cityName[j]!='\0';j++)
                            cD->cityName[j]=p2->cityName[j];
                        //for(int j=0;p2->objName[j]!='\0';j++)
                            cD->objName="/Users/Abhishek/Desktop/houseA_obj/houseA_obj.obj";
                            //cD->objName[j]=p2->objName[j];
                        for(int j=0;j<5;j++)
                            cD->houseCost[j]=p2->houseCost[j];
                        for(int j=0;j<6;j++)
                            cD->rent[j]=p2->rent[j];
                        cD->lx=p2->x;
                        cD->lz=p2->z;
                        cD->ori=p2->o;
                        
                        M->cityList.push_back(cD);
                    }
                    printf("client: received '%s'\n",buf);
                    close(sockfd);
                }
                for(int i=0;i<M->totalHumanPlayers;)
                {
                    int sockfd = 0, numbytes;
                    char buf[256];
                    struct addrinfo hints, *servinfo, *p;
                    int rv;
                    char s[INET6_ADDRSTRLEN];
                    
                    memset(&hints, 0, sizeof hints);
                    hints.ai_family = AF_UNSPEC;
                    hints.ai_socktype = SOCK_STREAM;
                    if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                    {
                        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                        continue;
                        //return 1;
                    }
                    
                    for(p = servinfo; p != NULL; p = p->ai_next)
                    {
                        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                        {
                            perror("client: socket");
                            continue;
                        }
                        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                        {
                            close(sockfd);
                            perror("client: connect");
                            continue;
                        }
                        break;
                    }
                    if (p == NULL)
                    {
                        fprintf(stderr, "client: failed to connect\n");
                        continue;
                        //return 2;
                    }
                    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                    printf("client: connecting to %s\n", s);
                    freeaddrinfo(servinfo);
                    if ((numbytes = recv(sockfd, buf, 256-1, 0)) == -1)
                    {
                        perror("recv");
                        exit(1);
                    }
                    buf[numbytes] = '\0';
                    if(sizeof(buf)>0)
                    {
                        i++;
                        packet3 *p3=(packet3*)buf;
                        
                        Player *p=new Player;
                        
                        p->playerID=p3->playerID;
                        p->IP=new char[16];
                        p->playerName=new char[32];
                        p->money=M->startingMoney;
                        p->currentPosition=M->cityList[0];
                        
                        for(int j=0;p3->IP[j]!='\0';j++)
                            p->IP[j]=p3->IP[j];
                        for(int j=0;p3->playerName[j]!='\0';j++)
                            p->playerName[j]=p3->playerName[j];
                        
                        M->playerList.push_back(p);
                    }
                    printf("client: received '%s'\n",buf);
                    close(sockfd);
                }
                for(int k=0;k<M->playerList.size();k++)
                    if(M->playerList[k]->playerID==hPID)
                        M->homePlayer=M->playerList[k];
                nextNetworkState();
            }
        }
        else if(networkState==4 && hPID==1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            char *argv=M->getPointerByID(currentPlayer)->IP;
            bool flag=true;
            while (flag)
            {
                int sockfd = 0, numbytes;
                char buf[MAXDATASIZE];
                struct addrinfo hints, *servinfo, *p;
                int rv;
                char s[INET6_ADDRSTRLEN];
                
                memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                    continue;
                    //return 1;
                }
                
                for(p = servinfo; p != NULL; p = p->ai_next)
                {
                    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                    {
                        perror("client: socket");
                        continue;
                    }
                    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                    {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                    }
                    break;
                }
                if (p == NULL)
                {
                    fprintf(stderr, "client: failed to connect\n");
                    continue;
                    //return 2;
                }
                inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                printf("client: connecting to %s\n", s);
                freeaddrinfo(servinfo);
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                if(sizeof(buf)>0)
                {
                    rdm *R=(rdm*)buf;
                    rdm1=R->r1;
                    rdm2=R->r2;
                    //std::cout<<"$$$$$$$$$$$$$$$$$  "<<R->r1<<" "<<R->r2<<" "<<R->NS<<"\n";
                    if(R->NS==networkState)
                    {
                        flag=false;
                        while(d!=-100.0);
                        M->throwDice(rdm1, rdm2);
                        callBack(0);
                    }
                }
                printf("client: received '%s'\n",buf);
                close(sockfd);
            }
            diceState=true;
            nextNetworkState();
        }
        else if(networkState==5 && hPID!=1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            char *argv=M->serverIP;
            bool flag=true;
            while (flag)
            {
                int sockfd = 0, numbytes;
                char buf[MAXDATASIZE];
                struct addrinfo hints, *servinfo, *p;
                int rv;
                char s[INET6_ADDRSTRLEN];
                
                memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                    continue;
                    //return 1;
                }
                
                for(p = servinfo; p != NULL; p = p->ai_next)
                {
                    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                    {
                        perror("client: socket");
                        continue;
                    }
                    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                    {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                    }
                    break;
                }
                if (p == NULL)
                {
                    fprintf(stderr, "client: failed to connect\n");
                    continue;
                    //return 2;
                }
                inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                printf("client: connecting to %s\n", s);
                freeaddrinfo(servinfo);
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                if(sizeof(buf)>0)
                {
                    rdm *R=(rdm*)buf;
                    rdm1=R->r1;
                    rdm2=R->r2;
                    if(R->NS==networkState)
                    {
                        flag=false;
                        M->throwDice(rdm1, rdm2);
                        callBack(0);
                    }
                }
                printf("client: received '%s'\n",buf);
                close(sockfd);
            }
            //diceState=0;
            nextNetworkState();
        }
        else if(networkState==6 && hPID==1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            char *argv=M->getPointerByID(currentPlayer)->IP;
            bool flag=true;
            while (flag)
            {
                int sockfd = 0, numbytes;
                char buf[MAXDATASIZE];
                struct addrinfo hints, *servinfo, *p;
                int rv;
                char s[INET6_ADDRSTRLEN];
                
                memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                    continue;
                    //return 1;
                }
                
                for(p = servinfo; p != NULL; p = p->ai_next)
                {
                    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                    {
                        perror("client: socket");
                        continue;
                    }
                    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                    {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                    }
                    break;
                }
                if (p == NULL)
                {
                    fprintf(stderr, "client: failed to connect\n");
                    continue;
                    //return 2;
                }
                inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                printf("client: connecting to %s\n", s);
                freeaddrinfo(servinfo);
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                if(sizeof(buf)>0)
                {
                    gamePacket *gP1=(gamePacket*)buf;
                    getGamePacket(gP1);
                    if(gP1->NS==networkState)
                    {
                        flag=false;
                        packetPlay();
                        callBack(0);
                    }
                }
                printf("client: received '%s'\n",buf);
                close(sockfd);
            }
            //diceState=0;
            nextNetworkState();
        }
        else if(networkState==7 && hPID!=1)
        {
            std::cout<<"NSC : "<<networkState<<"\n";
            char *argv=M->serverIP;
            bool flag=true;
            while (flag)
            {
                int sockfd = 0, numbytes;
                char buf[MAXDATASIZE];
                struct addrinfo hints, *servinfo, *p;
                int rv;
                char s[INET6_ADDRSTRLEN];
                
                memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                if ((rv = getaddrinfo(argv, PORT, &hints, &servinfo)) != 0)
                {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                    continue;
                    //return 1;
                }
                
                for(p = servinfo; p != NULL; p = p->ai_next)
                {
                    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
                    {
                        perror("client: socket");
                        continue;
                    }
                    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
                    {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                    }
                    break;
                }
                if (p == NULL)
                {
                    fprintf(stderr, "client: failed to connect\n");
                    continue;
                    //return 2;
                }
                inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
                printf("client: connecting to %s\n", s);
                freeaddrinfo(servinfo);
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                if(sizeof(buf)>0)
                {
                    gamePacket *gP1=(gamePacket*)buf;
                    getGamePacket(gP1);
                    if(gP1->NS==networkState)
                    {
                        flag=false;
                        packetPlay();
                        callBack(0);
                    }
                }
                printf("client: received '%s'\n",buf);
                close(sockfd);
            }
            //diceState=0;
            nextNetworkState();
        }
        if(M->totalPlayersLeft==1)
            pthread_exit(NULL);
    }
    pthread_exit(NULL);
}
