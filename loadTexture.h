#ifndef __LOParser__loadTexture__
#define __LOParser__loadTexture__

#include <iostream>
#include <string.h>
#include <stdio.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class loadTexture
{
    int height,width,tid;
    unsigned short bitsPerPixel;
    unsigned char *t;
public:
    loadTexture(int);
    int getTid();
    void readTextureFile(char *);
    void printGLcommand();
    ~loadTexture();
};

#endif
