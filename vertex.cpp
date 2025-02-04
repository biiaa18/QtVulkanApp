#include "../qtvulkan-course-code-biiaa18/vertex.h"


//Vertex::Vertex() {}


ostream& operator<<(ostream& Os, const Vertex& Vrtx){
    Os<<std::fixed;
    //x, y, z then r,g,b  then u,v
    Os<<"("<<Vrtx.x<<", "<<Vrtx.y<<", "<<Vrtx.z<<" )";     //  "(...)"; more detailed in () there are other strings " ("...",   "...",   "...")  "
    Os<<"("<<Vrtx.r<<", "<<Vrtx.g<<", "<<Vrtx.b<<" )";
    Os<<"("<<Vrtx.u<<", "<<Vrtx.v<<" )";
    return Os;
}


istream& operator>>(istream&  Is, Vertex& Vrtx){
    //using 4 temporary variables that will read the parenthesis and commas
    char temp1, temp2, temp3, temp4;
    Is>>temp1>>Vrtx.x>>temp2>>Vrtx.y>>temp3>>Vrtx.z>>temp4;
    Is>>temp1>>Vrtx.r>>temp2>>Vrtx.g>>temp3>>Vrtx.b>>temp4;
    Is>>temp1>>Vrtx.u>>temp2>>Vrtx.v>>temp3;
    return Is;
}
