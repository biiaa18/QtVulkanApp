#version 440

layout(location = 0) in vec3 vColor;

layout(location = 0) out vec4 fragColor;

float ambient =1.0f; //ambient is for how dark/light it is
//QVector3D lightColor=

void main()
{
    fragColor = vec4(vColor, 1.0);
}
