#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
#include <time.h>
#include <pthread.h>
#include "gameModule.h"
#include "loadObj.h"
#include "loadTexture.h"
#include "botPlayer.h"
#include "server.h"
#include "client.h"
#include "gameplay.h"
#ifdef __APPLE__
#  include <GLUI/glui.h>
#else
#  include <GL/glui.h>
#endif

Game *M; //Pointer to Game object

std::vector<cityData*> cityListMenu; //List of city accessible by the homeplayer

int networkState=1;

int playerCount=0;

int currentPlayer; //Player of the player whose turn is at present

pthread_t threads[2]; // Thread 1 for server and thread 2 for client

int state=3;

float dicesize=10.0;

int mainWindow;
GLUI *glui;
GLUI_Panel *panel2,*panel3,*panel4,*panel5,*panel6;
GLUI_Listbox *listBox1,*listBox2,*listBox3,*listBox4,*listBox5,*listBox6;
GLUI_Listbox *listBoxM1,*listBoxM2,*listBoxM3,*listBoxM4,*listBoxM5,*listBoxM6,*listBoxM7,*listBoxM8,*listBoxM9;
GLUI_RadioGroup *radioGroup1;

int texCount=0;
GLUquadric *quad;
loadTexture lT0(texCount++);
loadTexture lT1(texCount++);
loadTexture lT2(texCount++);
float height=10.0;
GLuint tex[32];
loadObj *lO;

char *cnum;
char *playerSelect;
int pS1=1,pS2=0,pS3=-1,taxType;
bool first=true;
float amount1,amount2,amount3;

int rdm1,rdm2;

bool diceState;

float h=0.0;

float d=-100.0;
float rx1=0.0,ry1=0.0,rz1=0.0;
float rx2=0.0,ry2=0.0,rz2=0.0;

float x = 0.0, z = -60.0;
float lx = 0.0, ly = 1.0 ,lz=1.0;

float deltaMovaUp = 0.0;
float deltaAnglex=0.0,deltaAngley=0.0;

float matAmb[][3]={{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0},{1.0,1.0,0.0},{0.0,1.0,1.0},{1.0,0.0,1.0}};
float matSpec[]={0.5,0.5,0.5};
float matDiff[]={0.5,0.5,0.5};

int frame=0,timebase=0,framespersec=0;

gamePacket gP;
//Structure containing windowing information
struct gultWindow
{
    int width;
    int height;
    char* title;
    float field_of_view_angle;
    float z_near;
    float z_far;
}win;
//Function to print fps
void fps()
{
    frame++;
    int time=glutGet(GLUT_ELAPSED_TIME);
    
    if (time - timebase > 1000)
    {
        framespersec = (frame*1000.0)/(time-timebase);
        timebase = time;
        frame = 0;
        std::cout<<framespersec<<"\n";
    }
    
}
//Function enabled when a key is pressed
void processNormalKeys(unsigned char key,int x, int y)
{
    switch(key)
    {
        case 27:
            exit ( 0 );
            break;
        case 'v':
            height+=80;
            height=(int)height%400;
    }
}
//Function enabled when arrow keys are pressed
void pressSpecialKey(int key, int xx, int yy)
{
    switch (key)
    {
        case GLUT_KEY_UP :
            deltaMovaUp = 1.0;
            break;
        case GLUT_KEY_DOWN :
            deltaMovaUp = -1.0;
            break;
        case GLUT_KEY_LEFT :
            break;
        case GLUT_KEY_RIGHT :
            break;
    }
}
//Function enabled when arrow keys are released
void releaseSpecialKey(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP :
            deltaMovaUp = 0.0;
            break;
        case GLUT_KEY_DOWN :
            deltaMovaUp = 0.0;
            break;
        case GLUT_KEY_LEFT :
            break;
        case GLUT_KEY_RIGHT :
            break;
    }
}
//Function enabled when cursor is moved
void mouseMove(int x, int y)
{
    deltaAnglex = x * 0.007;
    deltaAngley = y * 0.007;
    lx = -sin(deltaAnglex) ;
    lz = cos(deltaAnglex);
    ly = 2*cos(deltaAngley/2);
}
//Function enabled when mouse button is pressed
void mouseButton(int button, int s, int x, int y)
{
    callBack(0);
    if(!GLUT_LEFT_BUTTON && state==0 && d==-100.0)
    {
        M->throwDice(-1,-1);
        diceState=true;
        initializeGamePacket();
    }
}
void update(void)
{
    if (deltaMovaUp)
    {
        deltaAnglex = win.width * 0.01;
        deltaAngley = win.height * 0.01;
        x += deltaMovaUp * lx * 20.0;
        z += deltaMovaUp * lz * 20.0;
    }
    glutPostRedisplay();
}
//Function to display buttons
void gameButtons()
{
    GLUI_Master.set_glutKeyboardFunc(processNormalKeys );
    GLUI_Master.set_glutSpecialFunc(pressSpecialKey);
    GLUI_Master.set_glutMouseFunc( mouseButton );
    //GLUI_Master.set_glutReshapeFunc(myGlutReshape);
    glui = GLUI_Master.create_glui_subwindow(mainWindow,GLUI_SUBWINDOW_RIGHT);
    
    playerSelect=new char[128];
    
    listBox6=glui->add_listbox("Current Player ");
    listBox6->add_item(0,M->playerList[0]->playerName);
    
    listBox1=glui->add_listbox("My Money : ");
    listBox1->add_item(0,ita(M->homePlayer->money));
    
    panel2=glui->add_panel("Players ");
    listBox2=glui->add_listbox_to_panel(panel2,"Player      ",&pS1,1,callBack);
    listBoxM9=glui->add_listbox_to_panel(panel2,"Location : ");
    listBox3=glui->add_listbox_to_panel(panel2,"Cards       ");
    glui->add_button_to_panel(panel2,"Report Cheating",2,callBack);
    
    listBox5=glui->add_listbox("    Move  ",&pS3,11,callBack);
    
    panel3=glui->add_panel("Cards ");
    listBox4=glui->add_listbox_to_panel(panel3,"Name        ",&pS2,3,callBack);
    listBoxM1=glui->add_listbox_to_panel(panel3,"Status :      ");
    listBoxM2=glui->add_listbox_to_panel(panel3,"Owner :     ");
    listBoxM3=glui->add_listbox_to_panel(panel3,"Cost :        ");
    listBoxM8=glui->add_listbox_to_panel(panel3,"Rent :        ");
    listBoxM4=glui->add_listbox_to_panel(panel3,"Mortgage : ");
    glui->add_button_to_panel(panel3,"Mortgage/Buy Back",10,callBack);
    
    panel4=glui->add_panel("Current City ");
    listBoxM5=glui->add_listbox_to_panel(panel4,"   Name : ");
    glui->add_edittext_to_panel(panel4,"Money",GLUI_EDITTEXT_FLOAT,&amount1);
    glui->add_button_to_panel(panel4,"Buy City",4,callBack);
    glui->add_button_to_panel(panel4,"Build House",5,callBack);
    glui->add_button_to_panel(panel4,"Build Hotel",6,callBack);
    glui->add_button_to_panel(panel4,"Pay Rent",7,callBack);
    
    
    panel5=glui->add_panel("Tax ");
    glui->add_edittext_to_panel(panel5,"Money",GLUI_EDITTEXT_FLOAT,&amount2);
    radioGroup1=glui->add_radiogroup_to_panel(panel5,&taxType);
    glui->add_radiobutton_to_group(radioGroup1,"Amount ");
    glui->add_radiobutton_to_group(radioGroup1,"Percent ");
    glui->add_button_to_panel(panel5,"Pay Tax",8,callBack);
    
    
    panel6=glui->add_panel("Jail Fine ");
    glui->add_edittext_to_panel(panel6,"Money",GLUI_EDITTEXT_FLOAT,&amount3);
    glui->add_button_to_panel(panel6,"Pay Jail Fine",9,callBack);
    
    listBoxM7=glui->add_listbox("Message ");
    
    glui->set_main_gfx_window(mainWindow);
    
    delete playerSelect;
}
int alt, i,j;
cityData *cD;
//Function to draw players tokens
void drawUFO()
{
    cD =NULL;
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,matSpec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matDiff);
    for(i=0;i<M->playerList.size();i++)
    {
        alt=100;
        glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matAmb[i]);
        cD=M->playerList[i]->currentPosition;
        glPushMatrix();
        for(j=0;j<i;j++)
            if(cD==M->playerList[j]->currentPosition)
                alt+=25;
        if(cD!=NULL)
        {
            glTranslatef(cD->lx,alt,cD->lz);
            lO[1].drawObj(true);
        }
        glPopMatrix();
    }
}
//Function to draw cities
void drawCities()
{
    
    for(i=0;i<M->cityList.size();i++)
    {
        cD=M->cityList[i];
        glPushMatrix();
        glTranslatef(cD->lx,0,cD->lz);
        if(cD->ori!=0)
            glRotatef(cD->ori,0,1,0);
        lO[i+2].drawObj(false);
        glPopMatrix();
    }
}
float tc;
//Function to draw single road
void dr(float len,float r)
{
    tc=len/4;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,tc);
    glVertex3f(len/2,0.1,r);
    glTexCoord2f(1.0,tc);
    glVertex3f(len/2,0.1,-r);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-len/2.0,0.1,-r);
    glTexCoord2f(0.0,0.0);
    glVertex3f(-len/2.0,0.1,r);
    glEnd();
}

float dist,x1,z1,x2,z2,mx,mz,ang,sh;
//Function to draw roads
void drawRoads()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,tex[lT2.getTid()]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    

    
    sh=0.002;
    
    float tc=M->cityList.size();
    
    lT2.printGLcommand();
    
    for(i=0;i<tc;i++)
        for(j=0;j<tc;j++)
            if(M->graph[i][j]==true)
            {
                x1=M->cityList[i]->lx;
                z1=M->cityList[i]->lz;
                
                x2=M->cityList[j]->lx;
                z2=M->cityList[j]->lz;
                
                mx=(x1+x2)/2.0;
                mz=(z1+z2)/2.0;
                
                dist=sqrtf((x1-x2)*(x1-x2)+(z1-z2)*(z1-z2));
                
                ang=atanf((z1-z2)/(x1-x2))*180/3.14159;
                
                glPushMatrix();
                glTranslatef(mx,sh,mz);
                glRotatef(ang,0,-1,0);
                dr(dist,30.0);
                glPopMatrix();
                
                sh+=0.002;
            }
    glDisable(GL_TEXTURE_2D);
}
//Function to draw roads at an angle
void drawRoadsAngle(int num)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,tex[lT2.getTid()]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    lT2.printGLcommand();
    
    if(num>1)
    {
        for(float ang=180.0/(num-1),iy=-90;iy<90.1;iy+=ang)
        {
            glPushMatrix();
            glRotatef(iy, 0, 1, 0);
            dr(250,10);
            glPopMatrix();
        }
    }
    
    float d=(num==1)?1000.0:10.0;
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.2*(d+1000));
    glVertex3f(9.0,0.2,d);
    glTexCoord2f(1.0,0.2*(d+1000));
    glVertex3f(-9.0,0.2,d);
    glTexCoord2f(1.0,0.0);
    glVertex3f(-9.0,0.2,-1000.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(9.0,0.2,-1000.0);
    glEnd();
    glFlush();
    glDisable(GL_TEXTURE_2D);
}
//Function to draw ground
void drawGround()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,tex[lT0.getTid()]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    lT0.printGLcommand(); //glTexImage2D(....)
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,100.0);
    glVertex3f(1000.0,0.0,1000.0);
    glTexCoord2f(0.0,0.0);
    glVertex3f(1000.0,0.0,-1000.0);
    glTexCoord2f(100.0,0.0);
    glVertex3f(-1000.0,0.0,-1000.0);
    glTexCoord2f(100.0,100.0);
    glVertex3f(-1000.0,0.0,1000.0);
    glEnd();
    glFlush();
    
    glDisable(GL_TEXTURE_2D);
}

GLfloat mat_zero [] ={0.0,0.0,0.0,0.0};
GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 4.0 };
GLfloat mat_emission[] = {1.0, 1.0, 1.0, 1.0};
//Function to draw sky
void drawSky()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,tex[lT1.getTid()]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    

    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glBindTexture(GL_TEXTURE_2D,tex[lT1.getTid()]);
    glBegin(GL_QUADS);
    glEnd();
    glFlush();
    
    //glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT);
    
    quad=gluNewQuadric();
    gluQuadricTexture(quad,GL_TRUE);
    gluQuadricNormals(quad,GLU_SMOOTH);
    lT1.printGLcommand();
    glPushMatrix();
    glRotatef(-90,1,0,0);
    gluSphere(quad,1000, 50, 50);
    glPopMatrix();
    
    glFlush();
    glDisable(GL_TEXTURE_2D);
    
}

float hei;
//Function to draw dice
void drawDice()
{
    hei=(10.0*dicesize*exp(d)*sin(h*3.14159/180.0)>0)?(dicesize*(1+10.0*exp(d)*sin(h*3.14159/180.0))):(dicesize*(1-10.0*exp(d)*sin(h*3.14159/180.0)));
    
    glPushMatrix();
    glTranslatef(-2.0*dicesize,hei,0.0);
    glRotatef(rx1,1,0,0);
    glRotatef(ry1,0,1,0);
    glRotatef(rz1,0,0,1);
    //glScalef(dicesize,dicesize,dicesize);
    lO[0].drawObj(false);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(2.0*dicesize,hei,0.0);
    glRotatef(rx2,1,0,0);
    glRotatef(ry2,0,1,0);
    glRotatef(rz2,0,0,1);
    //glScalef(dicesize,dicesize,dicesize);
    lO[0].drawObj(false);
    glPopMatrix();
    
    d=(d>-90*0.03)?(d-0.03):-100;
    
    if(d>-90*0.03)
    {
        h=h+10.0;
        
        rx1=rx1+12;
        ry1=ry1+4;
        rz1=rz1+8;
        
        rx2=rx2+8;
        ry2=ry2+4;
        rz2=rz2+8;
    }
}
//The main display function
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    
    gluLookAt(x,height,z,x+lx,height+ly,z+lz,0.0,1.0,0.0);//from at direction
    
    drawSky();
    
    drawGround();
    
    drawCities();
    
    drawDice();
    
    drawUFO();
    
    drawRoads();
    
    glutSwapBuffers();
    
    //fps();
}
//Function to initialize openGL
void initialize ()
{
    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, win.width, win.height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat) win.width / win.height;
    gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);
    
    glMatrixMode(GL_MODELVIEW);
    glShadeModel( GL_SMOOTH );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    glEnable(GL_LIGHT0);
    
    glEnable(GL_LIGHTING);
    
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    
    GLfloat light0_position[] = {0.0, 100.0, 0.0, 1.0 };
    GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };
    GLfloat amb_light[] = {1.0,1.0,1.0,1.0};
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb_light);
    glLightfv(GL_LIGHT0,GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR, specular);
    //glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    //glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
    //glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
    //glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(32,tex);
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
}
//Function to initialize game
void gameInitialize()
{
    int opt;
    
    std::cout<<"Server/Client(1/2) : ";
    std::cin>>opt;
    char *hPN=new char[128];
    
    if(opt==1)
    {
        std::cout<<"Total Human Players : ";
        std::cin>>M->totalHumanPlayers;
        std::cout<<"Total Bot Players : ";
        std::cin>>M->totalBotPlayers;
        std::cout<<"Name : ";
        std::cin>>hPN;
        if(M->totalHumanPlayers>1)
        {
            std::cout<<"Server IP : ";
            M->serverIP=new char[16];
            std::cin>>M->serverIP;
        }
        Player *p=new Player;
        p->playerName=hPN;
        p->money=M->startingMoney;
        p->playerID=1;
        p->IP=M->serverIP;
        p->playerID=1;
        p->currentPosition=M->cityList[0];
        M->homePlayer=p;
        M->playerList.push_back(p);
    }
    else
    {
        std::cout<<"Name : ";
        std::cin>>hPN;
        std::cout<<"Server IP : ";
        M->serverIP=new char[16];
        std::cin>>M->serverIP;
        M->homePlayer=new Player;
        M->homePlayer->playerName=hPN;
        M->homePlayer->playerID=0;
    }
    
    M->totalPlayersLeft=M->totalBotPlayers+M->totalHumanPlayers;
    
    currentPlayer=1;
    
}
//Function to initialize remaining openGL
void openGLInitialize(int argc, char **argv)
{
    lO=new loadObj[M->cityList.size()+2];
    //lO[0].readObjFile("/Users/Abhishek/Desktop/Cities/Dice_Low_Poly_OBJ/Dice_LowPoly_OBJ.obj");
    lO[0].readObjFile("/Users/Abhishek/Desktop/Cities/dole7k2dc3y8-Dicehighpoly/Dice_high_poly/Dice_high_poly.obj",true);
    lO[1].readObjFile("/Users/Abhishek/Desktop/Cities/fg0fbvkvr7r4-UFO/UFO/UFO.obj",false);

    for(int i=0;i<M->cityList.size();i++)
        lO[i+2].readObjFile(M->cityList[i]->objName,false);
    
    lT0.readTextureFile("/Users/Abhishek/Desktop/grass.bmp");
    lT1.readTextureFile("/Users/Abhishek/Desktop/darksky.bmp");
    lT2.readTextureFile("/Users/Abhishek/Desktop/road.bmp");
    
    win.width = 1280;
    win.height = 720;
    win.title = "MONOPOLY";
    win.field_of_view_angle = 60;
    win.z_near = 1.0f;
    win.z_far = 2000.0f;
    
    glutInit(&argc,argv);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize(win.width,win.height);
    mainWindow=glutCreateWindow(win.title);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutFullScreen();
    
    initialize();
    
    gameButtons();
    
    glutIgnoreKeyRepeat(1);
    
    glutMouseFunc(mouseButton);
    glutPassiveMotionFunc(mouseMove);
    
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(pressSpecialKey);
    glutSpecialUpFunc(releaseSpecialKey);
}
//Function to initialize parameters
void parInit()
{
    M=new Game;
    M->currency=new char[16];
    M->currency="Rupees";
    M->jailFine=20;
    M->taxAlternate=30;
    M->taxPercent=10;
    M->totalCities=16;
    M->startingMoney=500;
    
    for(int i=0;i<M->totalCities;i++)
        M->graph[i][(i+1)%M->totalCities]=true;
    
    M->graph[4][10]=true;
    
    cityData *cD;
    
    int mfx[4]={1,0,-1,0};
    int mfz[4]={0,1,0,-1};
    
    int len=200;
    
    int ix=-(len*3)/2;
    int iz=-(len*3)/2;
    
    for(int i=0;i<16;i++)
    {
        cD=new cityData;
        cD->locationID=i;
        cD->groupID=i%4;
        cD->cityName=new char[16];
        cD->cityName[0]=cD->cityName[1]=cD->cityName[2]=i+65;
        cD->objName=new char[128];
        cD->objName="/Users/Abhishek/Desktop/houseA_obj/houseA_obj.obj";
        cD->cityCost=30*i;
        cD->mortgageValue=10*i;
        for(int j=1;j<=5;j++)
            cD->houseCost[j-1]=10*j+5*i;
        for(int j=1;j<=6;j++)
            cD->rent[j-1]=3*j+2*i;
        M->cityList.push_back(cD);
        cD->lx=ix;
        cD->lz=iz;
        ix=ix+mfx[i/4]*len;
        iz=iz+mfz[i/4]*len;
        //cD->ori=90.0*(i/4);
    }
    
}
//The main function
int main(int argc, char **argv)
{
    parInit();
    
    gameInitialize();
    
    pthread_create(&threads[0],NULL,server,NULL);
    
    pthread_create(&threads[1],NULL,client,NULL);
    
    while(networkState<=3);
    
    for(int i=0;i<M->totalBotPlayers;i++)
    {
        char *bn=new char[6];
        bn[0]='B'; bn[1]='O'; bn[2]='T';
        strcat(bn,ita(i+1));
        Player *p=new Player;
        p->playerName=bn;
        p->money=M->startingMoney;
        p->playerID=(-(i+1));
        p->currentPosition=M->cityList[0];
        M->playerList.push_back(p);
    }
    
    openGLInitialize(argc,argv);
    
    //callBack(0);
    
    for(int i=0;i<M->cityList.size();i++)
        listBox4->add_item(i,M->cityList[i]->cityName);
    for(int i=0;i<M->playerList.size();i++)
        listBox2->add_item(M->playerList[i]->playerID, M->playerList[i]->playerName);
    
    if(M->totalHumanPlayers==1)
        nextNetworkState();
    
    glutMainLoop();
    
    return 0;
}
