#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H
#include <vector>
#include <string>
#include "Vertex.h"
#include "visualobject.h"

class TriangleSurface : public VisualObject
{
public:
    //std::vector<Vertex> mVertices;
    //std::vector<Vertex> getVertices() { return mVertices; }
    TriangleSurface();
    TriangleSurface(const std::string& filename);

    float radius{};
};

#endif // TRIANGLESURFACE_H
