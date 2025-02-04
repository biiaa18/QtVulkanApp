#ifndef VERTEX_H
#define VERTEX_H
#include <iostream>
#include <ostream>
using namespace std;

struct Vertex
{
public:
        // Vertex();

    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    float u;
    float v;

    //overloaded ostream, which WRITES all vertex data from open textfile
    friend ostream& operator<<(ostream&, const Vertex&);

    //overloaded istream, which READS all vertex data from open textfile
    friend istream& operator>>(istream&, Vertex&);
};

#endif // VERTEX_H
