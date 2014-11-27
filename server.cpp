#include "server.h"

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

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr_S(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *server(void *n)
{
    int hPID=M->homePlayer->playerID;
    const char *PORT="3490";
    if(hPID==1)
        PORT="3490";
    else
        PORT="3491";
    int sockfd = 0, new_fd = 0;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        //return 1;
    }
    
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        //return 2;
    }
    
    freeaddrinfo(servinfo);
    
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
    
    printf("server: waiting for connections...\n");
    
    while(1)
    {
        if(networkState>3)
            hPID=M->homePlayer->playerID;
        if(networkState==1 && hPID==1)
        {
            std::cout<<"NS : "<<networkState<<"\n";
            int i=2;
            while(i<=M->totalHumanPlayers)
            {
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                
                inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                printf("server: got connection from %s\n", s);
                int j;
                for(j=0;j<M->playerList.size();j++)
                    if(strncmp(s,M->playerList[j]->IP,sizeof(s))==0)
                        break;
                //if(j==M->playerList.size())
                {
                    Player *p=new Player;
                    p->playerID=i;
                    p->money=M->startingMoney;
                    p->IP=new char[sizeof(s)];
                    for(int k=0;k<sizeof(s);k++)
                        p->IP[k]=s[k];
                    p->playerID=i;
                    p->currentPosition=M->cityList[0];
                    M->playerList.push_back(p);
                    
                    if (!fork())
                    {
                        char *cnum=new char[2];
                        std::stringstream ss;
                        ss<<i;
                        ss>>cnum;
                        close(sockfd);
                        if (send(new_fd, cnum, 2, 0) == -1)
                            perror("send");
                        delete cnum;
                        close(new_fd);
                        exit(0);
                    }
                    i++;
                }
                close(new_fd);  // parent doesn't need this
            }
            networkState=2;
        }
        else if(networkState==2 && hPID!=1)
        {
            std::cout<<"NS : "<<networkState<<"\n";
            bool flag=true;
            while(flag)
            {
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                
                inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                printf("server: got connection from %s\n", s);
                
                if(strncmp(s,M->serverIP,sizeof(s))==0)
                {
                    if (!fork())
                    {
                        close(sockfd);
                        if (send(new_fd,M->homePlayer->playerName,sizeof(M->homePlayer->playerName), 0) == -1)
                            perror("send");
                        close(new_fd);
                        exit(0);
                    }
                    flag=false;
                }
                close(new_fd);
            }
            networkState=3;
        }
        else if(networkState==3 &&  hPID==1)
        {
            std::cout<<"NS : "<<networkState<<"\n";
            int i=2;
            while(i<=M->totalHumanPlayers)
            {
                {
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    
                    if (new_fd == -1)
                    {
                        perror("accept");
                        continue;
                    }
                    inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                    printf("server: got connection from %s\n", s);
                    
                    if (!fork())
                    {
                        packet1 p1;
                        for(int i=0;i<32;i++)
                            for(int j=0;j<32;j++)
                                p1.graph[i][j]=M->graph[i][j];
                        p1.jailFine=M->jailFine;
                        p1.startingMoney=M->startingMoney;
                        p1.taxAlternate=M->taxAlternate;
                        p1.taxPercent=M->taxPercent;
                        p1.totalBotPlayers=M->totalBotPlayers;
                        p1.totalCities=M->totalCities;
                        p1.totalHumanPlayers=M->totalHumanPlayers;
                        for(int j=0;M->currency[j]!='\0';j++)
                            p1.currency[j]=M->currency[j];
                        
                        close(sockfd);
                        if (send(new_fd, &p1,sizeof(p1), 0) == -1)
                            perror("send");
                        close(new_fd);
                        exit(0);
                    }
                    close(new_fd);
                }
                for(int i=0;i<M->totalCities;i++)
                {
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    
                    if (new_fd == -1)
                    {
                        perror("accept");
                        continue;
                    }
                    inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                    printf("server: got connection from %s\n", s);
                    
                    if(!fork())
                    {
                        packet2 p2;
                        p2.special=M->cityList[i]->special;
                        p2.locationID=M->cityList[i]->locationID;
                        p2.groupID=M->cityList[i]->groupID;
                        p2.mortgageValue=M->cityList[i]->mortgageValue;
                        p2.cityCost=M->cityList[i]->cityCost;
                        for(int j=0;M->cityList[i]->cityName[j]!='\0';j++)
                            p2.cityName[j]=M->cityList[i]->cityName[j];
                        for(int j=0;M->cityList[i]->objName[j]!='\0';j++)
                            p2.objName[j]=M->cityList[i]->objName[j];
                        for(int j=0;j<5;j++)
                            p2.houseCost[j]=M->cityList[i]->houseCost[j];
                        for(int j=0;j<6;j++)
                            p2.rent[j]=M->cityList[i]->rent[j];
                        p2.x=M->cityList[i]->lx;
                        p2.z=M->cityList[i]->lz;
                        p2.o=M->cityList[i]->ori;
                        
                        close(sockfd);
                        if (send(new_fd, &p2,sizeof(p2), 0) == -1)
                            perror("send");
                        close(new_fd);
                        exit(0);
                    }
                    close(new_fd);
                }
                
                for(int i=0;i<M->totalHumanPlayers;i++)
                {
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    
                    if (new_fd == -1)
                    {
                        perror("accept");
                        continue;
                    }
                    inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                    
                    printf("server: got connection from %s\n", s);
                    
                    if(!fork())
                    {
                        packet3 p3;
                        p3.playerID=M->playerList[i]->playerID;
                        for(int j=0;M->playerList[i]->playerName[j]!='\0';j++)
                            p3.playerName[j]=M->playerList[i]->playerName[j];
                        for(int j=0;M->playerList[i]->IP[j]!='\0';j++)
                            p3.IP[j]=M->playerList[i]->IP[j];
                        
                        close(sockfd);
                        if (send(new_fd, &p3,sizeof(p3), 0) == -1)
                            perror("send");
                        close(new_fd);
                        exit(0);
                    }
                    close(new_fd);
                }
                i++;
            }
            nextNetworkState();
        }
        else if(networkState==4 && hPID!=1 && currentPlayer==hPID && state==4)
        {
            std::cout<<"@@@@NS : "<<networkState<<"\n";
            rdm R;
            R.r1=rdm1;
            R.r2=rdm2;
            R.NS=networkState;
            bool flag=true;
            while(flag)
            {
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                
                inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                printf("server: got connection from %s\n", s);
                
                if(strncmp(s,M->serverIP,sizeof(s))==0)
                {
                    if (!fork())
                    {
                        close(sockfd);
                        if (send(new_fd,&R,sizeof(R), 0) == -1)
                            perror("send");
                        close(new_fd);
                        exit(0);
                    }
                    
                }
                close(new_fd);
                flag=false;
            }
            while(d!=-100.0);
            nextNetworkState();
        }
        else if(networkState==5 && hPID==1 && ((currentPlayer>0 && state==4) || currentPlayer<0))
        {
            std::cout<<"$$$$NS : "<<networkState<<"\n";
            rdm R;
            R.r1=rdm1;
            R.r2=rdm2;
            R.NS=networkState;
            for(int i=0;i<M->totalHumanPlayers;)
            {
                if(M->getPointerByID(i+1)->playerID==currentPlayer || M->getPointerByID(i+1)->playerID==1)
                {
                    i++;
                    continue;
                }
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                
                printf("server: got connection from %s\n", s);
                
                if(!fork())
                {
                    std::cout<<"DICE : "<<R.r1<<" "<<R.r2<<"\n";
                    close(sockfd);
                    if (send(new_fd, &R,sizeof(R), 0) == -1)
                        perror("send");
                    close(new_fd);
                    exit(0);
                }
                close(new_fd);
                i++;
            }
            nextNetworkState();
        }
        else if(networkState==6 && hPID!=1 && state==3 && currentPlayer==hPID)
        {
            std::cout<<"????NS : "<<networkState<<"\n";
            gP.NS=networkState;
            bool flag=true;
            while(flag)
            {
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                
                inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                printf("server: got connection from %s\n", s);
                
                if(strncmp(s,M->serverIP,sizeof(s))==0)
                {
                    if (!fork())
                    {
                        close(sockfd);
                        if (send(new_fd,&gP,sizeof(gP), 0) == -1)
                            perror("send");
                        close(new_fd);
                        exit(0);
                    }
                    
                }
                close(new_fd);
                flag=false;
            }
            nextNetworkState();
        }
        else if(networkState==7 && hPID==1 && (currentPlayer!=hPID || (currentPlayer==hPID && state==3)))
        {
            std::cout<<"****NS : "<<networkState<<"\n";
            std::cout<<currentPlayer<<" "<<state<<"\n";
            gP.NS=networkState;
            for(int i=0;i<M->totalHumanPlayers;)
            {
                if(M->getPointerByID(i+1)->playerID==currentPlayer || M->getPointerByID(i+1)->playerID==1)
                {
                    i++;
                    continue;
                }
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                
                if (new_fd == -1)
                {
                    perror("accept");
                    continue;
                }
                inet_ntop(their_addr.ss_family,get_in_addr_S((struct sockaddr *)&their_addr),s, sizeof s);
                
                printf("server: got connection from %s\n", s);
                
                if(!fork())
                {
                    close(sockfd);
                    if (send(new_fd, &gP,sizeof(gP), 0) == -1)
                        perror("send");
                    close(new_fd);
                    exit(0);
                }
                close(new_fd);
                i++;
            }
            nextNetworkState();
        }
        if(M->totalPlayersLeft==1)
            pthread_exit(NULL);
    }
    pthread_exit(NULL);
}
