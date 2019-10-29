#include "colorshader.h"
#include "innpch.h"

ColorShader::ColorShader(const GLchar *geometryPath)
    : Shader("plainshader", geometryPath) {
    mMatrixUniform = glGetUniformLocation(program, "mMatrix");
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");
}

ColorShader::~ColorShader() {
    qDebug() << "Deleting ColorShader";
}
