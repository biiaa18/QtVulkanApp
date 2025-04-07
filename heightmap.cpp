#include "heightmap.h"

HeightMap::HeightMap() {

}

void HeightMap::makeTerrain(unsigned char* textureData,float grid_size, float step) {
    float x_min=0.0;  //use it as VertexXStart
    float y_min=0.0;
    unsigned short x_max=grid_size;
    unsigned short y_max=grid_size;
    float vertexZStart=0.0f;


    //MAKE VERTICES FIRST
    for (auto x=0; x<x_max; x+=step){   // was x++, y++
        for (auto y=0; y<y_max; y+=step){
            int index = (x + y * y_max) * 4; // Each pixel has 4 bytes (RGBA)
            float heightFromBitmap = static_cast<float>(textureData[index]);         // * heightSpacing + heightPlacement;
            //                                      x - value                      y-value               z-value
            mVertices.emplace_back(Vertex{x_min + (y * step), heightFromBitmap, vertexZStart - (x * step),
                                          //  R , G, B                    U, V
                                          0.0f,0.5f,0.0f,           y / (y_max - 1.f), x / (x_max - 1.f)});
        }
    }

    // CONNECT VERTICES (TRIANGULATE THE PLANE) USING INDICES
    for(auto x=0; x<x_max; x+=step)  //was x++, y++ before
    {
        for(auto y=0; y<y_max; y+=step)
        {
            //Indices for one quad:
            mIndices.emplace_back(y + x * y_max);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(y + x * y_max + y_max + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.emplace_back(y + x * y_max + y_max);       // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.emplace_back(y + x * y_max);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(y + x * y_max + 1);           // 0 + 0 * mWidth + 1           = 1
            mIndices.emplace_back(y + x * y_max + y_max + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }
}
