#version 330 core

uniform sampler2D qt_Texture0;
in vec4 qt_TexCoord0;
out vec4 gl_FragColor;

void main(void)
{
    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
}
