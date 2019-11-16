#version 330 core
layout(location = 0) in vec3 posAttr;

out vec3 TexCoords;

uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main() {
   TexCoords = posAttr;
   vec4 pos = pMatrix * vMatrix * vec4(posAttr, 1.0);
   gl_Position = pos.xyww;
}
