#ifndef __LOParser__loadObj__
#define __LOParser__loadObj__

#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "loadTexture.h"
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


extern int texCount;
extern GLuint tex[32];
extern float dicesize;

class loadObj
{
    int toNor,toVer,toGroups,toMat;
    struct coordinates;
    struct faces;
    struct material;
    bool materialFile;
    std::vector<loadTexture*> textureList;
    std::vector<material*> materialList;
    std::vector<int> materialListGroup;
    std::vector<bool> smooth;
    std::vector<int> cummToFaces;
    std::vector<int> toFaces;
    std::vector<int> noVer;
    std::vector<int> type;
    std::vector<coordinates*> vertices;
    std::vector<coordinates*> normals;
    std::vector<coordinates*> texels;
    std::vector<faces*> faceList;
    std::vector<std::vector<faces*> > faceList2D;
    std::vector<coordinates*> normalList2D[4];
public:
    loadObj();
    void readObjFile(char *,bool);
    int readMatFile(char *);
    void drawObj(bool ufo);
    void calculateNormal(coordinates,coordinates,coordinates,int);
    ~loadObj();
};

#endif
