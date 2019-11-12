attribute vec4 qt_Vertex;
attribute vec4 qt_MultiTexCoord0;
uniform mat4 qt_ModelViewProjectionMatrix;
varying vec4 qt_TexCoord0;

void main(void)
{
    gl_Position = pMatrix * mMatrix * vec4(aPos, 1.0);
    qt_TexCoord0 = qt_MultiTexCoord0;
    (glDisable(GL_DEPTH_TEST)
}
