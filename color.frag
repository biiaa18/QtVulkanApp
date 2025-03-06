#version 440

layout(location = 0) in vec3 v_color;

layout(location = 0) out vec4 fragColor;

float ambient =1.0f; //ambient is for how dark/light it is
//QVector3D lightColor=

void main()
{
    fragColor = vec4(v_color, 1.0)*ambient;
}
