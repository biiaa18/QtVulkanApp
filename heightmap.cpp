#include "heightmap.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

HeightMap::HeightMap():VisualObject()
{

   //makeTerrain("D:\\HeightMap\\Heightmap.jpg", 256, 256);
   makeTerrain("D:\\HeightMap\\Heightmap.jpg", 256, 0.1, 0.055);
   //mMatrix.rotate(45, 0.0, 0.0, 0.0f);

}

void HeightMap::makeTerrain(const char* textureData, int widthIn, int heightIn)
{
    unsigned char* TextureData = reinterpret_cast<unsigned char*>(stbi_load(textureData, &widthIn, &heightIn, nullptr, 4));
    if(!TextureData)
    {
        qDebug() << "Failed to load heightmap image!";
        return;
    }
    //Default normal pointing straight up - should be calculated correctly for lights to work!!!
    float normal[3]{0.f, 0.5f, 0.f};

    //How many meters between each vertex in both x and z direction
    //This should be sent in as a parameter!
    float horisontalSpacing{.5f};

    //Scaling the height read from the heightmap. 0 -> 255 meters if this is set to 1
    //This should be sent in as a parameter!
    float heightSpacing{.02f};

    //Offset the whole terrain in y (height) axis
    //Moves the terrain mesh up or down
    //Because of Barycentric calculations, we want the terrain to be in World coordinates!
    //So we don't want to move the terrain up or down in the Y axis after it is made
    float heightPlacement{-5.f};

    //Getting the scale of the heightmap
    //Using depth as the name of texture height, to not confuse with terrain height
    unsigned short width = widthIn;       //Width == x-axis
    unsigned short depth = heightIn;      //Depth == z-axis

    //Temp variables for creating the mesh
    //Adding offset so the middle of the terrain will be in World origo
    float vertexXStart{0.f};            // if world origo should be at center use: {0.f - width * horisontalSpacing / 2};
    float vertexZStart{0.f};            // if world origo should be at center use: {0.f + depth * horisontalSpacing / 2};

    //Loop to make the mesh from the values read from the heightmap (textureData)
    //Double for-loop to make the depth and the width of the terrain in one go
    for(int d{0}; d < depth; ++d)       //depth loop
    {
        for(int w{0}; w < width; ++w)   //width loop
        {
            //Heightmap image is actually stored as an one dimentional array - so calculating the correct index for column and row
            //and scale it according to variables
            // Calculate the correct index for the R value of each pixel
            int index = (w + d * width) * 4; // Each pixel has 4 bytes (RGBA)
            if (index >= widthIn * heightIn * 4) // Extra safety check
            {
                qDebug() << "Index out of bounds:" << index;
                return;
            }
            float heightFromBitmap = static_cast<float>(TextureData[index]);         // * heightSpacing + heightPlacement;
            //                                          x - value                      y-value               z-value
            mVertices.emplace_back(Vertex{vertexXStart + (w * horisontalSpacing), heightFromBitmap, vertexZStart - (d * horisontalSpacing),
                                          //  dummy normal=0,1,0                  Texture coordinates
                                          normal[0],normal[1],normal[2],           w / (width - 1.f), d / (depth - 1.f)});
        }
    }

    // The mesh(grid) is drawn in quads with diagonals from lower left to upper right
    //          _ _
    //         |/|/|
    //          - -
    //         |/|/|
    //          - -
    //Making the indices for this mesh:
    for(int d{0}; d < depth-1; ++d)        //depth - 1 because we draw the last quad from depth - 1 and in negative z direction
    {
        for(int w{0}; w < width-1; ++w)    //width - 1 because we draw the last quad from width - 1 and in positive x direction
        {
            //Indices for one quad:
            mIndices.emplace_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.emplace_back(w + d * width + width);       // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.emplace_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * width + 1);           // 0 + 0 * mWidth + 1           = 1
            mIndices.emplace_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }

}


void HeightMap::makeTerrain(const char* textureData,int grid_size, float horisontal_space, float vertical_space) {
    unsigned char* TextureData = reinterpret_cast<unsigned char*>(stbi_load(textureData, &grid_size, &grid_size, nullptr, 4));
    if(!TextureData)
    {
        qDebug() << "Failed to load heightmap image!";
        return;
    }

    int x_min=0;
    int z_min=0;
    unsigned short x_max=grid_size; //just the max boundaries of the input image
    unsigned short z_max=grid_size; //just the max boundaries of the input image
    float heightPlacement{10.f};

    float vertexXStart{0.f};            // if world origo should be at center use: {0.f - width * horisontalSpacing / 2};
    float vertexZStart{0.f};            // if world origo should be at center use: {0.f + depth * horisontalSpacing / 2};


    //MAKE VERTICES FIRST
    for (auto x=0; x<x_max; x++){   // was x++, y++
        for (auto z=0; z<z_max; z++){
            int index = (z + x * z_max) * 4; // Each pixel has 4 bytes (RGBA)
            if (index >= grid_size * grid_size * 4) // Extra safety check
            {
                qDebug() << "Index out of bounds:" << index;
                return;
            }
            float heightFromBitmap = static_cast<float>(TextureData[index]);         // * heightSpacing + heightPlacement;
            //                                      x - value                      y-value               z-value
            mVertices.emplace_back(Vertex{vertexXStart + (z * horisontal_space), heightFromBitmap*vertical_space, vertexZStart - (x * horisontal_space),
                                          //  R , G, B                    U, V
                                          0.0f,0.5f, 0.0f,           z / (z_max - 1.f), x / (x_max - 1.f)});
        }
    }

    // CONNECT VERTICES (TRIANGULATE THE PLANE) USING INDICES
    for(auto x=0; x<x_max-1; x++)
    {
        for(auto z=0; z<z_max-1; z++)
        {
            //Indices for one quad:
            mIndices.emplace_back(z + x * z_max);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(z + x * z_max + z_max + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.emplace_back(z + x * z_max + z_max);       // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.emplace_back(z + x * z_max);               // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(z + x * z_max + 1);           // 0 + 0 * mWidth + 1           = 1
            mIndices.emplace_back(z + x * z_max + z_max + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }
}
