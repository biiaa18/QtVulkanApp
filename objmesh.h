#ifndef OBJMESH_H
#define OBJMESH_H
#include "VisualObject.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QDebug>
#include <QVector3D>
using namespace std;



class ObjMesh :public VisualObject
{
public:
    ObjMesh(const string& filename);

private:
    bool readObjFile(const string& filename);
};

#endif // OBJMESH_H
