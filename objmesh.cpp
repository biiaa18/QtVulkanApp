#include "objmesh.h"
#include "VisualObject.h"
#include <string.h>
using namespace std;

ObjMesh::ObjMesh(const string& filename)
{
    drawType=1;
    if (!readObjFile(filename))  //If file does not read, just make a triangle
    {
        mVertices.push_back(Vertex{ -0.5f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f });
        mVertices.push_back(Vertex{ -0.5f,   -0.5f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f });
        mVertices.push_back(Vertex{ 0.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f });
        qDebug("Made you a triangle instead...");
    }

    mMatrix.translate(1.f, 0, 0);
}

bool ObjMesh::readObjFile(const string &filename)
{
    string tempName{};

    tempName = "D:\\Meshes\\" + filename;
    //tempName = "D:\\Meshes\\sphere.obj";    // "D:\Meshes\sphere.obj"
    qDebug() << "Reading " << tempName.c_str();
    ifstream fileIn;
    fileIn.open(tempName, ifstream::in);
    if (!fileIn)
    {
        qDebug() << "ERROR: Could not open file for reading: " << filename.c_str();
        return false;
    }
    //One line at a time-variable
    string oneLine{};
    //One word at a time-variable
    string oneWord{};

    // temp variables to store the data from the file
    vector<QVector3D> tempVertecies;
    vector<QVector3D> tempNormals;
    vector<QVector2D> tempUVs;

    // Varible for constructing the indices vector
    // All obj files use indices
    unsigned int temp_index = 0;

    //Reading one line at a time from file to oneLine
    while (getline(fileIn, oneLine))
    {
        //Doing a trick to get one word at a time
        stringstream sStream;
        //Pushing line into stream
        sStream << oneLine;
        //Streaming one word out of line
        oneWord = ""; //resetting the value or else the last value might survive!
        sStream >> oneWord;

        if (oneWord == "#")
        {
            //Ignore this line
            //qDebug() << "Line is comment "  << QString::fromStdString(oneWord);
            continue;
        }
        if (oneWord == "")
        {
            //Ignore this line
            //qDebug() << "Line is blank ";
            continue;
        }
        ////////////////rn ignoring the shaders
        if (oneWord == "s")
        {
            //Ignore this line
            //qDebug() << "Line is blank ";
            continue;
        }
        if (oneWord == "v")
        {
            //qDebug() << "Line is vertex "  << QString::fromStdString(oneWord) << " ";
            QVector3D tempVertex;
            sStream >> oneWord;
            tempVertex.setX(stof(oneWord)); //string to float  stof
            sStream >> oneWord;
            tempVertex.setY(stof(oneWord));
            sStream >> oneWord;
            tempVertex.setZ(stof(oneWord));

            //Vertex made - pushing it into vertex-vector
            tempVertecies.push_back(tempVertex);

            continue;   //jump out of the while loop
        }
        if (oneWord == "vt")
        {
            //qDebug() << "Line is UV-coordinate "  << QString::fromStdString(oneWord) << " ";
            QVector2D tempUV;
            sStream >> oneWord;
            tempUV.setX(stof(oneWord));
            sStream >> oneWord;
            tempUV.setY(stof(oneWord));

            //UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            continue;
        }
        if (oneWord == "vn")
        {
            //qDebug() << "Line is normal "  << QString::fromStdString(oneWord) << " ";
            QVector3D tempNormal;
            sStream >> oneWord;
            tempNormal.setX(stof(oneWord));
            sStream >> oneWord;
            tempNormal.setY(stof(oneWord));
            sStream >> oneWord;
            tempNormal.setZ(stof(oneWord));

            //Vertex made - pushing it into vertex-vector
            tempNormals.push_back(tempNormal);
            continue;
        }
        if (oneWord == "f")
        {
            //qDebug() << "Line is a face "  << QString::fromStdString(oneWord) << " ";
            int index, normal, uv;
            for (int i = 0; i < 3; i++) //I<3 , BECAUSE WE HAVE TRIANGULATED MESH, SO FOR EVERY 3 VERTICES
            {
                sStream >> oneWord;                             //one word read
                stringstream tempWord(oneWord);            //to use getline on this one word
                string segment;                            //the numbers in the f-line
                vector<string> segmentArray;          //temp array of the numbers
                while (getline(tempWord, segment, '/'))    //splitting word in segments at /
                {
                    segmentArray.push_back(segment);
                }
                index = stoi(segmentArray[0]);             //first is vertex  //STOI - STRING TO INTEGER
                if (segmentArray[1] != "")                      //second is uv
                    uv = stoi(segmentArray[1]);
                else
                {
                    qDebug() << "No UVs in mesh!!!";            //uv not present
                    uv = 0;                                     //this will become -1 in a couple of lines
                }
                normal = stoi(segmentArray[2]);            //third is normal

                //Fixing the indexes
                //because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1)    //uv present!
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], tempUVs[uv]);
                    mVertices.push_back(tempVert);
                }
                else            //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], QVector2D(0.0f, 0.0f));
                    mVertices.push_back(tempVert);
                }
                //We have now handeled one Vertex on the f-line - add it to indices
                mIndices.push_back(temp_index++);
            }
            continue;
        }
    }

    fileIn.close();

    qDebug() << filename.c_str() << " successfully loaded";
    return true;
}
