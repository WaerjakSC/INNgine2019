QT          += core gui widgets

TEMPLATE    = app
CONFIG      += c++17

TARGET      = INNgine2019

PRECOMPILED_HEADER = innpch.h

INCLUDEPATH +=  ./GSL

HEADERS += \
    GSL/matrix2x2.h \
    GSL/matrix3x3.h \
    GSL/matrix4x4.h \
    GSL/vector2d.h \
    GSL/vector3d.h \
    GSL/vector4d.h \
    GSL/gsl_math.h \
    GSL/math_constants.h \
    Shaders/colorshader.h \
    Shaders/textureshader.h \
    Shaders/phongshader.h \
    Shaders/shader.h \
    constants.h \
    renderwindow.h \
    resourcemanager.h \
    mainwindow.h \
    triangle.h \
    texture.h \
    billboard.h \
    vertex.h \
    xyz.h \
    visualobject.h \
    octahedronball.h \
    billboard.h \
    camera.h \
    octahedronball.h \
    gltypes.h \
    trianglesurface.h \
    input.h \
    material.h \
    light.h \
    objmesh.h \
#    innpch.h \
    skybox.h \


SOURCES += main.cpp \
    GSL/matrix2x2.cpp \
    GSL/matrix3x3.cpp \
    GSL/matrix4x4.cpp \
    GSL/vector2d.cpp \
    GSL/vector3d.cpp \
    GSL/vector4d.cpp \
    GSL/gsl_math.cpp \
    Shaders/colorshader.cpp \
    Shaders/textureshader.cpp \
    Shaders/phongshader.cpp \
    Shaders/shader.cpp \
    renderwindow.cpp \
    mainwindow.cpp \
    resourcemanager.cpp \
    triangle.cpp \
    texture.cpp \
    billboard.cpp \
    vertex.cpp \
    visualobject.cpp \
    camera.cpp \
    octahedronball.cpp \
    xyz.cpp \
    trianglesurface.cpp \
    input.cpp \
    material.cpp \
    light.cpp \
    objmesh.cpp \
    skybox.cpp

FORMS += \
    mainwindow.ui

DISTFILES += \
    Shaders/phongshader.frag \
    Shaders/phongshader.vert \
    Shaders/plainshader.frag \
    Shaders/plainshader.vert \
    Shaders/textureshader.frag \
    GSL/README.md \
    README.md \
    Shaders/textureshader.vert
