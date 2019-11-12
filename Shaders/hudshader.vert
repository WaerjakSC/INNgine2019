#version 330 core

layout(location = 0) in vec4 qt_Vertex;
layout(location = 1) in vec4 qt_MultiTexCoord0;
uniform mat4 mMatrix;
uniform mat4 pMatrix;
out vec4 qt_TexCoord0;

void main(void)
{
    gl_Position = pMatrix * mMatrix * vec4(aPos, 1.0);
    qt_TexCoord0 = qt_MultiTexCoord0;
}
