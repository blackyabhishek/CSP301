#include "loadTexture.h"
//Constructor of class loadTexture
loadTexture::loadTexture(int id)
{
    tid=id;
    width=0;
    height=0;
    bitsPerPixel=0;
    t=NULL;
}
//Destructor of class loadTexture
loadTexture::~loadTexture()
{
    delete []t;
}
int loadTexture::getTid()
{
    return tid;
}
//Function to read texture file
void loadTexture::readTextureFile(char * filename)
{
    FILE* fin=fopen(filename,"r");
    if(!fin)
    {
        std::cout<<"Wrong Texture File!\n";
    }
    else
    {
        unsigned int fileSize,startingAddress,imageSize;
        
        fseek(fin,2,SEEK_SET);
        fread(&fileSize,1,4,fin);
        fseek(fin,10,SEEK_SET);
        fread(&startingAddress,1,4,fin);
        fseek(fin,18,SEEK_SET);
        fread(&width,1,4,fin);
        fseek(fin,22,SEEK_SET);
        fread(&height,1,4,fin);
        fseek(fin,28,SEEK_SET);
        fread(&bitsPerPixel,1,2,fin);
        fseek(fin,34,SEEK_SET);
        fread(&imageSize,1,4,fin);
        t=new unsigned char[imageSize];
        fseek(fin,startingAddress,SEEK_SET);
        fread(t,imageSize,1,fin);
        char temp;
        
        if(bitsPerPixel==24)
        {
            imageSize=imageSize-imageSize%3;
            for(int i=0;i<imageSize;i++)
                if(i%3==0)
                {
                    temp=t[i];
                    t[i]=t[i+2];
                    t[i+2]=temp;
                }
        }
        else if(bitsPerPixel==16)
        {
            for(int i=0;i<imageSize;i++)
                if(i%2==0)
                {
                    temp=t[i];
                    t[i]=(t[i] & 11110000)|(t[i+1] & 00001111);
                    t[i+1]=(t[i+1] & 11110000)|(temp & 00001111);
                }
        }
        else if(bitsPerPixel==32)
        {
            for(int i=0;i<imageSize;i++)
                if(i%4==0)
                {
                    temp=t[i];
                    t[i]=t[i+1];
                    t[i+1]=t[i+2];
                    t[i+2]=temp;
                }
        }
    }
    std::cout<<"Tex File : "<<bitsPerPixel<<" : "<<filename<<"\n";
    fclose(fin);
}

void loadTexture::printGLcommand()
{
    if(bitsPerPixel==24)
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,t);
    else if(bitsPerPixel==4)
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGB4,GL_UNSIGNED_BYTE,t);
    else if(bitsPerPixel==8)
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGB8,GL_UNSIGNED_BYTE,t);
    else if(bitsPerPixel==16)
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGB16,GL_UNSIGNED_BYTE,t);
    else if(bitsPerPixel==32)
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,t);
}
