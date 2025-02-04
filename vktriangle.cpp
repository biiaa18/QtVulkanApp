#include "vktriangle.h"

VkTriangle::VkTriangle() : VisualObject() {
    // Y up, front = CCW
    //               X,     Y,     Z,       R,    G,    B,       U,  V
    Vertex obj1_vrtx{-1.0f, 0.0f,  0.0f,     1.0f, 0.0f, 0.0f,    0.0f, 0.0f};  //we now made first Vertex object, so we define floats we had in the vertex header here
    Vertex obj2_vrtx{1.0f, 0.0f,  0.0f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f};
    Vertex obj3_vrtx{0.0f, 1.0f,  0.0f,     0.0f, 0.0f, 0.0f,    0.0f, 0.0f};
    mVertices.push_back(Vertex{-1.0f, 0.0f,  0.0f,     1.0f, 0.0f, 0.0f,    0.0f, 0.0f});  //we are pushing vertex 1 data into the vector "myVertices" we made
    mVertices.push_back(Vertex{1.0f, 0.0f,  0.0f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f});
    mVertices.push_back(Vertex{0.0f, 1.0f,  0.0f,     0.0f, 0.0f, 0.0f,    0.0f, 0.0f});

    // Flytter trekanten litt til venstre og lagrer translasjonen i en 4x4 matrise
    //mMatrix.translate(-0.25f, 0, 0);

    //  Skalerer ned kvadratet i eget koordinatsystem/frame
    //mMatrix.scale(0.25f);

}



