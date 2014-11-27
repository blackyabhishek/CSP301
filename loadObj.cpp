
#include "loadObj.h"

//Constructor for loadObj
loadObj::loadObj()
{
    toNor=0;
    toVer=0;
    toGroups=0;
    cummToFaces.push_back(0);
    materialFile=false;
}
//Destructor for loadObj
loadObj::~loadObj()
{
    smooth.erase(smooth.begin(),smooth.end());
    cummToFaces.erase(cummToFaces.begin(),cummToFaces.end());
    toFaces.erase(toFaces.begin(),toFaces.end());
    noVer.erase(noVer.begin(),noVer.end());
    type.erase(type.begin(),type.end());
    materialList.erase(materialList.begin(),materialList.end());
    textureList.erase(textureList.begin(),textureList.end());
    vertices.erase (vertices.begin(),vertices.end());
    normals.erase (normals.begin(),normals.end());
    texels.erase (texels.begin(),texels.end());
    materialListGroup.erase(materialListGroup.begin(),materialListGroup.end());
    faceList2D.erase (faceList2D.begin(),faceList2D.end());
    for(int i=3;i>=0;i--)
        normalList2D[i].erase (normalList2D[i].begin(),normalList2D[i].end());
}
//Structure to store material data
struct loadObj::material
{
    char *matName;
    struct colorMat;
    float Ka[3],Kd[3],Ks[3],Tf[3];
    int illum,d,Ns,sharpness,Ni;
    char *fnKa,*fnKd,*fnKs,*fnNs,*fnd,*fndecal,*fndisp,*fnbump;
    int tidKa,tidKd,tidKs,tidNs,tidd,tiddecal,tiddisp,tidbump;
    bool aat;
    material()
    {
        matName=fnKa=fnKd=fnKs=fnNs=fnd=fndecal=fndisp=fnbump=NULL;
        aat=false;
        illum=2;
    }
};
//Structure to store coordinates
struct loadObj::coordinates
{
    coordinates(float x,float y)
    {
        xc=x;
        yc=y;
    }
    coordinates(float x,float y,float z)
    {
        xc=x;
        yc=y;
        zc=z;
    }
    float xc,yc,zc;
};
//Structure to store the faces of object
struct loadObj::faces
{
    faces(int vc1,int tc1,int nc1,int vc2,int tc2,int nc2,int vc3,int tc3,int nc3,int vc4,int tc4,int nc4)
    {
        v1=vc1;
        t1=tc1;
        n1=nc1;
        v2=vc2;
        t2=tc2;
        n2=nc2;
        v3=vc3;
        t3=tc3;
        n3=nc3;
        v4=vc4;
        t4=tc4;
        n4=nc4;
    }
    /*faces(int size)
    {
        vp1=new int[size];
        np2=new int[size];
        tp3=new int[size];
    }
    int *vp1,*np2,*tp3;*/
    int v1,t1,n1,v2,t2,n2,v3,t3,n3,v4,t4,n4;
};
//Function to calculate normals
void loadObj::calculateNormal(coordinates c1,coordinates c2,coordinates c3,int i)
{
    
    float va[3], vb[3], vr[3], val;
    va[0] = c1.xc - c2.xc;
    va[1] = c1.yc - c2.yc;
    va[2] = c1.zc - c2.zc;
    
    vb[0] = c1.xc - c3.xc;
    vb[1] = c1.xc - c3.yc;
    vb[2] = c1.xc - c3.zc;
    
    vr[0] = va[1] * vb[2] - vb[1] * va[2];
    vr[1] = vb[0] * va[2] - va[0] * vb[2];
    vr[2] = va[0] * vb[1] - vb[0] * va[1];
    
    val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
    
    coordinates norm(vr[0]/val,vr[1]/val,vr[2]/val);
    
    normalList2D[i].push_back(&norm);
}
//Function too read object file
void loadObj::readObjFile(char *objFileName,bool a)
{
    std::cout<<"Obj File : "<<objFileName<<"\n";
    std::ifstream fin(objFileName,std::ios::in);
    if(!fin.is_open())
    {
        std::cout<<"Wrong Object File!\n";
    }
    else
    {
        bool check=true;
        int pos = 0;
        for(int i=(int)strlen(objFileName)-1;i>=0;i--)
            if(objFileName[i]=='/')
            {
                pos=i;
                break;
            }
        float x1,y1,z1;
        int v1,t1,n1,v2,t2,n2,v3,t3,n3,v4,t4,n4;
        char *line;
        char *mfn;
        bool shade=true,set=true;
        coordinates *coord;
        faces *face;
        noVer.push_back(0);
        type.push_back(0);
        toFaces.push_back(0);
        while(fin)
        {
            line=new char[256];
            mfn=new char[128];
            fin.getline(line,1024);
            if(line[0]=='o')
            {
                if(toGroups!=0)
                {
                    check=true;
                    noVer.push_back(0);
                    type.push_back(0);
                    toFaces.push_back(0);
                }
            }
            else if(line[0]=='v' && line[1]==' ')
            {
                toVer++;
                sscanf(line,"v %f %f %f",&x1,&y1,&z1);
                if(a)
                    coord=new coordinates(dicesize*x1,dicesize*y1,dicesize*z1);
                else
                    coord=new coordinates(x1,y1,z1);
                vertices.push_back(coord);
            }
            else if(line[0]=='v' && line[1]=='n')
            {
                toNor++;
                sscanf(line,"vn %f %f %f",&x1,&y1,&z1);
                coord=new coordinates(x1,y1,z1);
                normals.push_back(coord);
            }
            else if(line[0]=='v' && line[1]=='t')
            {
                sscanf(line,"vt %f %f",&x1,&y1);
                coord=new coordinates(x1,y1);
                texels.push_back(coord);
            }
            else if(line[0]=='f')
            {
                if(check)
                {
                    type[toGroups-1]=0;
                    for(int i=1;line[i]!='\0';i++)
                    {
                        if(line[i]==' ' && line[i-1]!=' ')
                            noVer[toGroups-1]=noVer[toGroups-1]+1;
                        else if(line[i]=='/')
                        {
                            type[toGroups-1]=type[toGroups-1]+1;
                            if(line[i+1]=='/')
                                type[toGroups-1]=-1000;
                        }
                    }
                    if(type[toGroups-1]==noVer[toGroups-1])
                        type[toGroups-1]=1; //110
                    else if(type[toGroups-1]<0)
                        type[toGroups-1]=2; //101
                    else if(type[toGroups-1]>noVer[toGroups-1])
                        type[toGroups-1]=3; //111
                    check=false;
                }
                toFaces[toGroups-1]=toFaces[toGroups-1]+1;
                cummToFaces[toGroups-1]=cummToFaces[toGroups-1]+1;
                if(noVer[toGroups-1]==3)
                {
                    if(type[toGroups-1]==0)
                    {
                        sscanf(line,"f %d %d %d",&v1,&v2,&v3);
                        face=new faces(v1,-1,-1,v2,-1,-1,v3,-1,-1,-1,-1,-1);
                        calculateNormal(*vertices[v1-1],*vertices[v2-1],*vertices[v3-1],0);
                        calculateNormal(*vertices[v2-1],*vertices[v3-1],*vertices[v1-1],1);
                        calculateNormal(*vertices[v3-1],*vertices[v1-1],*vertices[v2-1],2);
                    }
                    else if(type[toGroups-1]==1)
                    {
                        sscanf(line,"f %d/%d %d/%d %d/%d",&v1,&t1,&v2,&t2,&v3,&t3);
                        face=new faces(v1,t1,-1,v2,t2,-1,v3,t3,-1,-1,-1,-1);
                        calculateNormal(*vertices[v1-1],*vertices[v2-1],*vertices[v3-1],0);
                        calculateNormal(*vertices[v2-1],*vertices[v3-1],*vertices[v1-1],1);
                        calculateNormal(*vertices[v3-1],*vertices[v1-1],*vertices[v2-1],2);
                    }
                    else if(type[toGroups-1]==2)
                    {
                        sscanf(line,"f %d//%d %d//%d %d//%d",&v1,&n1,&v2,&n2,&v3,&n3);
                        face=new faces(v1,-1,n1,v2,-1,n2,v3,-1,n3,-1,-1,-1);
                    }
                    else if(type[toGroups-1]==3)
                    {
                        sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3);
                        face=new faces(v1,t1,n1,v2,t2,n2,v3,t3,n3,-1,-1,-1);
                    }
                }
                else// if(noVer[toGroups-1]==4)
                {
                    if(type[toGroups-1]==0)
                    {
                        sscanf(line,"f %d %d %d %d",&v1,&v2,&v3,&v4);
                        face=new faces(v1,-1,-1,v2,-1,-1,v3,-1,-1,v4,-1,-1);
                        calculateNormal(*vertices[v1-1],*vertices[v2-1],*vertices[v4-1],0);
                        calculateNormal(*vertices[v2-1],*vertices[v3-1],*vertices[v1-1],1);
                        calculateNormal(*vertices[v3-1],*vertices[v4-1],*vertices[v2-1],2);
                        calculateNormal(*vertices[v4-1],*vertices[v1-1],*vertices[v3-1],3);
                    }
                    else if(type[toGroups-1]==1)
                    {
                        sscanf(line,"f %d/%d %d/%d %d/%d %d/%d",&v1,&t1,&v2,&t2,&v3,&t3,&v4,&t4);
                        face=new faces(v1,t1,-1,v2,t2,-1,v3,t3,-1,v4,t4,-1);
                        calculateNormal(*vertices[v1-1],*vertices[v2-1],*vertices[v4-1],0);
                        calculateNormal(*vertices[v2-1],*vertices[v3-1],*vertices[v1-1],1);
                        calculateNormal(*vertices[v3-1],*vertices[v4-1],*vertices[v2-1],2);
                        calculateNormal(*vertices[v4-1],*vertices[v1-1],*vertices[v3-1],3);
                    }
                    else if(type[toGroups-1]==2)
                    {
                        sscanf(line,"f %d//%d %d//%d %d//%d %d//%d",&v1,&n1,&v2,&n2,&v3,&n3,&v4,&n4);
                        face=new faces(v1,-1,n1,v2,-1,n2,v3,-1,n3,v4,-1,n4);
                    }
                    else if(type[toGroups-1]==3)
                    {
                        sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3,&v4,&t4,&n4);
                        face=new faces(v1,t1,n1,v2,t2,n2,v3,t3,n3,v4,t4,n4);
                    }
                }
                //else
                {
                    /*int *vp11=new int[noVer[toGroups-1]];
                    int *np11=new int[noVer[toGroups-1]];
                    int *tp11=new int[noVer[toGroups-1]];
                    if(type[toGroups-1]==2)
                    {
                        sscanf(line,"f %d//%d %d//%d %d//%d %d//%d",&v1,&n1,&v2,&n2,&v3,&n3,&v4,&n4);
                        face=new faces(noVer[toGroups-1]);
                        for(int i=0,c=0;i<strlen(line);i++)
                        {
                            
                        }
                    }
                    else if(type[toGroups-1]==3)
                    {
                        sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3,&v4,&t4,&n4);
                        face=new faces(v1,t1,n1,v2,t2,n2,v3,t3,n3,v4,t4,n4);
                    }*/
                }
                faceList.push_back(face);
            }
            else if(set && sscanf(line,"mtllib %s",mfn)==1)
            {
                int pos=0;
                for(int i=0;objFileName[i]!='\0';i++)
                    if(objFileName[i]=='/')
                        pos=i;
                char *matFileName=new char[128];
                for(int i=0;i<128;i++)
                    matFileName[i]='\0';
                strncpy(matFileName,objFileName,pos+1);
                strcat(matFileName,mfn);
                toMat=readMatFile(matFileName);
                set=false;
            }
            else if(sscanf(line,"usemtl %s",mfn)==1)
            {
                if(toGroups!=0)
                {
                    faceList2D.push_back(faceList);
                    cummToFaces.push_back(cummToFaces.back());
                    faceList.resize(0);
                    check=true;
                    noVer.push_back(0);
                    type.push_back(0);
                    toFaces.push_back(0);
                }
                smooth.push_back(shade);
                shade=true;
                toGroups++;
                for(int i=0;i<toMat;i++)
                {
                    if(strcmp(materialList[i]->matName,mfn)==0 )
                    {
                        materialListGroup.push_back(i);
                        if(materialList[i]->fnKd!=NULL)
                        {
                            char *path=new char[128];
                            strncpy(path,objFileName,pos);
                            if(materialList[i]->fnKd[0]!='/')
                                strcat(path,"/");
                            strncat(path,materialList[i]->fnKd,strlen(materialList[i]->fnKd)-4);
                            strcat(path,".bmp");
                            loadTexture *lT=new loadTexture(materialList[i]->tidKd);
                            lT->readTextureFile(path);
                            textureList.push_back(lT);
                        }
                        else
                        {
                            loadTexture *lT=new loadTexture(materialList[i]->tidKd);
                            textureList.push_back(lT);
                        }
                        break;
                    }
                }
            }
            else if(line[0]=='s')
            {
                if(strcmp(line,"s off")==0 || strcmp(line,"s 0")==0)
                    shade=false;
            }
            //else if(line[0]=='g')
            //std::cout<<line<<"\n";
            delete []line;
            delete []mfn;
        }
        faceList2D.push_back(faceList);
    }
    fin.close();
}
//Function to read material file
int loadObj::readMatFile(char * fileName)
{
    int count=0;
    char *line=NULL;
    char *temp=NULL;
    float tf[3];
    int ti[1];
    material *mat = NULL;
    std::cout<<"Mat File : "<<fileName<<"\n";
    std::ifstream fin(fileName,std::ios::in);
    if(!fin.is_open())
    {
        std::cout<<"Wrong Material File!\n";
    }
    else
    {
        materialFile=true;
        while(fin)
        {
            line=new char[256];
            fin.getline(line,256);
            temp=new char[128];
            if(sscanf(line,"newmtl %s",temp)==1)
            {
                if(count!=0)
                {
                    materialList.push_back(mat);
                }
                mat=new material;
                mat->matName=temp;
                count++;
            }
            
            //****//
            
            else if(sscanf(line,"Ka %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Ka[0]=tf[0];
                mat->Ka[1]=tf[1];
                mat->Ka[2]=tf[2];
            }
            else if(sscanf(line,"\tKa %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Ka[0]=tf[0];
                mat->Ka[1]=tf[1];
                mat->Ka[2]=tf[2];
            }
            else if(sscanf(line,"Kd %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Kd[0]=tf[0];
                mat->Kd[1]=tf[1];
                mat->Kd[2]=tf[2];
            }
            else if(sscanf(line,"\tKd %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Kd[0]=tf[0];
                mat->Kd[1]=tf[1];
                mat->Kd[2]=tf[2];
            }
            else if(sscanf(line,"Ks %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Ks[0]=tf[0];
                mat->Ks[1]=tf[1];
                mat->Ks[2]=tf[2];
            }
            else if(sscanf(line,"\tKs %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Ks[0]=tf[0];
                mat->Ks[1]=tf[1];
                mat->Ks[2]=tf[2];
            }
            else if(sscanf(line,"Tf %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Tf[0]=tf[0];
                mat->Tf[1]=tf[1];
                mat->Tf[2]=tf[2];
            }
            else if(sscanf(line,"\tTf %f %f %f",&tf[0],&tf[1],&tf[2])==3)
            {
                mat->Tf[0]=tf[0];
                mat->Tf[1]=tf[1];
                mat->Tf[2]=tf[2];
            }
            
            //****//
            
            else if(sscanf(line,"illumn %d",&ti[0])==1)
                mat->illum=ti[0];
            else if(sscanf(line,"d %d",&ti[0])==1)
                mat->d=ti[0];
            else if(sscanf(line,"Ns %d",&ti[0])==1)
                mat->Ns=ti[0];
            else if(sscanf(line,"Ni %d",&ti[0])==1)
                mat->Ni=ti[0];
            else if(sscanf(line,"sharpness %d",&ti[0])==1)
                mat->sharpness=ti[0];
            
            //****//
            
            else if(sscanf(line,"map_Ka %s",temp)==1)
            {
                mat->fnKa=temp;
                //mat->tidKa=texCount++;
            }
            else if(sscanf(line,"map_Kd %s",temp)==1)
            {
                mat->fnKd=temp;
                mat->tidKd=texCount++;
            }
            else if(sscanf(line,"\tmap_Kd %s",temp)==1)
            {
                mat->fnKd=temp;
                mat->tidKd=texCount++;
            }
            else if(sscanf(line,"map_Ks %s",temp)==1)
            {
                mat->fnKs=temp;
                //mat->tidKs=texCount++;
            }
            else if(sscanf(line,"map_Ns %s",temp)==1)
            {
                mat->fnNs=temp;
                //mat->tidNs=texCount++;
            }
            else if(sscanf(line,"map_d %s",temp)==1)
            {
                mat->fnd=temp;
                //mat->tidd=texCount++;
            }
            else if(sscanf(line,"decal %s",temp)==1)
            {
                mat->fndecal=temp;
                //mat->tiddecal=texCount++;
            }
            else if(sscanf(line,"disp %s",temp)==1)
            {
                mat->fndisp=temp;
                //mat->tiddisp=texCount++;
            }
            else if(sscanf(line,"bump %s",temp)==1)
            {
                mat->fnbump=temp;
                //mat->tidbump=texCount++;
            }
            else if(sscanf(line,"map_aat %s",temp)==1)
            {
                if(temp[1]=='n')
                    mat->aat=true;
                else
                    mat->aat=false;
            }
            //****//
            delete line;
        }
        materialList.push_back(mat);
    }
    fin.close();
    return count;
}
//Function to draw object file
void loadObj::drawObj(bool ufo)
{
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_DST_COLOR);
    for(int k=0,ind=-1;k<toGroups;k++)
    {
        if(smooth[k]==false)
            glShadeModel(GL_FLAT);
        else
            glShadeModel( GL_SMOOTH );
        
        if(materialFile && materialList[materialListGroup[k]]->fnKd!=NULL && !ufo)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,textureList[k]->getTid());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            textureList[k]->printGLcommand();
        }
 
        GLfloat matEmission[3]={0.0,0.0,0.0};
        GLfloat matShine[]={10.0};
        
        //std::cout<<k<<" "<<"MATERIAL "<<materialList[materialListGroup[k]]->matName<<"\n";
        //std::cout<<k<<" "<<"AMBIENT "<<materialList[materialListGroup[k]]->Ka[0]<<"\n";
        //std::cout<<k<<" "<<"DIFFUSIVE "<<materialList[materialListGroup[k]]->Kd[0]<<"\n";
        //std::cout<<k<<" "<<"SPECULAR "<<materialList[materialListGroup[k]]->Ks[0]<<"\n";
        
        if(materialFile && !ufo)
        {
            glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,materialList[materialListGroup[k]]->Ka);
            glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,materialList[materialListGroup[k]]->Ks);
            glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,materialList[materialListGroup[k]]->Kd);
        }
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
        
        if(noVer[k]==3)
        {
            glBegin(GL_TRIANGLES);
            if(type[k]==0 || (materialFile && type[k]==1 && materialList[materialListGroup[k]]->fnKd==NULL))
            {
                ind=(k>1)?cummToFaces[k-1]-1:0;
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normalList2D[0][ind+i]->xc,normalList2D[0][ind+i]->yc,normalList2D[0][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normalList2D[1][ind+i]->xc,normalList2D[1][ind+i]->yc,normalList2D[1][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normalList2D[2][ind+i]->xc,normalList2D[2][ind+i]->yc,normalList2D[2][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                }
            }
            else if(type[k]==1)
            {
                ind=(k>1)?cummToFaces[k-1]-1:0;
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normalList2D[0][ind+i]->xc,normalList2D[0][ind+i]->yc,normalList2D[0][ind+i]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t1-1]->xc,texels[faceList2D[k][i]->t1-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normalList2D[1][ind+i]->xc,normalList2D[1][ind+i]->yc,normalList2D[1][ind+i]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t2-1]->xc,texels[faceList2D[k][i]->t2-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normalList2D[2][ind+i]->xc,normalList2D[2][ind+i]->yc,normalList2D[2][ind+i]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t3-1]->xc,texels[faceList2D[k][i]->t3-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                }
            }
            else if(type[k]==2 || (materialFile && type[k]==3 && materialList[materialListGroup[k]]->fnKd==NULL))
            {
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normals[faceList2D[k][i]->n1-1]->xc,normals[faceList2D[k][i]->n1-1]->yc,normals[faceList2D[k][i]->n1-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n2-1]->xc,normals[faceList2D[k][i]->n2-1]->yc,normals[faceList2D[k][i]->n2-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n3-1]->xc,normals[faceList2D[k][i]->n3-1]->yc,normals[faceList2D[k][i]->n3-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                }
            }
            else if(type[k]==3)
            {
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normals[faceList2D[k][i]->n1-1]->xc,normals[faceList2D[k][i]->n1-1]->yc,normals[faceList2D[k][i]->n1-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t1-1]->xc,texels[faceList2D[k][i]->t1-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n2-1]->xc,normals[faceList2D[k][i]->n2-1]->yc,normals[faceList2D[k][i]->n2-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t2-1]->xc,texels[faceList2D[k][i]->t2-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n3-1]->xc,normals[faceList2D[k][i]->n3-1]->yc,normals[faceList2D[k][i]->n3-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t3-1]->xc,texels[faceList2D[k][i]->t3-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                }
            }
        }
        else
        {
            glBegin(GL_QUADS);
            if(type[k]==0 || (materialFile && type[k]==1 && materialList[materialListGroup[k]]->fnKd==NULL))
            {
                ind=(k>1)?cummToFaces[k-1]-1:0;
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normalList2D[0][ind+i]->xc,normalList2D[0][ind+i]->yc,normalList2D[0][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normalList2D[1][ind+i]->xc,normalList2D[1][ind+i]->yc,normalList2D[1][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normalList2D[2][ind+i]->xc,normalList2D[2][ind+i]->yc,normalList2D[2][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                    glNormal3f(normalList2D[3][ind+i]->xc,normalList2D[3][ind+i]->yc,normalList2D[3][ind+i]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v4-1]->xc,vertices[faceList2D[k][i]->v4-1]->yc,vertices[faceList2D[k][i]->v4-1]->zc);
                }
            }
            else if(type[k]==1)
            {
                ind=(k>1)?cummToFaces[k-1]-1:0;
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normalList2D[0][ind+i]->xc,normalList2D[0][ind+i]->yc,normalList2D[0][ind+i]->zc);
                    glTexCoord2f(texels[faceList[i]->t1-1]->xc,texels[faceList[i]->t1-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normalList2D[1][ind+i]->xc,normalList2D[1][ind+i]->yc,normalList2D[1][ind+i]->zc);
                    glTexCoord2f(texels[faceList[i]->t2-1]->xc,texels[faceList[i]->t2-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normalList2D[2][ind+i]->xc,normalList2D[2][ind+i]->yc,normalList2D[2][ind+i]->zc);
                    glTexCoord2f(texels[faceList[i]->t3-1]->xc,texels[faceList[i]->t3-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                    glNormal3f(normalList2D[3][ind+i]->xc,normalList2D[3][ind+i]->yc,normalList2D[3][ind+i]->zc);
                    glTexCoord2f(texels[faceList[i]->t4-1]->xc,texels[faceList[i]->t4-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v4-1]->xc,vertices[faceList2D[k][i]->v4-1]->yc,vertices[faceList2D[k][i]->v4-1]->zc);
                }
            }
            else if(type[k]==2 || (materialFile && type[k]==3 && materialList[materialListGroup[k]]->fnKd==NULL))
            {
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normals[faceList2D[k][i]->n1-1]->xc,normals[faceList2D[k][i]->n1-1]->yc,normals[faceList2D[k][i]->n1-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n2-1]->xc,normals[faceList2D[k][i]->n2-1]->yc,normals[faceList2D[k][i]->n2-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n3-1]->xc,normals[faceList2D[k][i]->n3-1]->yc,normals[faceList2D[k][i]->n3-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n4-1]->xc,normals[faceList2D[k][i]->n4-1]->yc,normals[faceList2D[k][i]->n4-1]->zc);
                    glVertex3f(vertices[faceList2D[k][i]->v4-1]->xc,vertices[faceList2D[k][i]->v4-1]->yc,vertices[faceList2D[k][i]->v4-1]->zc);
                }
            }
            else if(type[k]==3)
            {
                for(int i=0;i<toFaces[k];i++)
                {
                    glNormal3f(normals[faceList2D[k][i]->n1-1]->xc,normals[faceList2D[k][i]->n1-1]->yc,normals[faceList2D[k][i]->n1-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t1-1]->xc,texels[faceList2D[k][i]->t1-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v1-1]->xc,vertices[faceList2D[k][i]->v1-1]->yc,vertices[faceList2D[k][i]->v1-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n2-1]->xc,normals[faceList2D[k][i]->n2-1]->yc,normals[faceList2D[k][i]->n2-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t2-1]->xc,texels[faceList2D[k][i]->t2-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v2-1]->xc,vertices[faceList2D[k][i]->v2-1]->yc,vertices[faceList2D[k][i]->v2-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n3-1]->xc,normals[faceList2D[k][i]->n3-1]->yc,normals[faceList2D[k][i]->n3-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t3-1]->xc,texels[faceList2D[k][i]->t3-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v3-1]->xc,vertices[faceList2D[k][i]->v3-1]->yc,vertices[faceList2D[k][i]->v3-1]->zc);
                    glNormal3f(normals[faceList2D[k][i]->n4-1]->xc,normals[faceList2D[k][i]->n4-1]->yc,normals[faceList2D[k][i]->n4-1]->zc);
                    glTexCoord2f(texels[faceList2D[k][i]->t4-1]->xc,texels[faceList2D[k][i]->t4-1]->yc);
                    glVertex3f(vertices[faceList2D[k][i]->v4-1]->xc,vertices[faceList2D[k][i]->v4-1]->yc,vertices[faceList2D[k][i]->v4-1]->zc);
                }
            }
        }
        glDisable(GL_TEXTURE_2D);
        if(materialFile && materialList[materialListGroup[k]]->fnKd!=NULL && !ufo)
        {
            glDisable(GL_TEXTURE_2D);
        }
    }
    glEnd();
    glFlush();
    glDisable(GL_TEXTURE_2D);
}
