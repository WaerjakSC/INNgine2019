QT          += core gui widgets

TEMPLATE    = app
CONFIG      += c++17

TARGET      = INNgine2019

PRECOMPILED_HEADER = innpch.h

INCLUDEPATH += ./GSL ./Components ./Shaders ./Systems ./Views .\GUI .\Sound

win32 {
    INCLUDEPATH += $(OPENAL_HOME)\\include\\AL
    LIBS *= $(OPENAL_HOME)\\libs\\Win64\\libOpenAL32.dll.a
}

HEADERS += \
    Components/lightcomponent.h \
    Components/materialcomponent.h \
    Components/physicscomponent.h \
    Components/soundcomponent.h \
    Components/transformcomponent.h \
    GSL/matrix2x2.h \
    GSL/matrix3x3.h \
    GSL/matrix4x4.h \
    GSL/vector2d.h \
    GSL/vector3d.h \
    GSL/vector4d.h \
    GSL/gsl_math.h \
    GSL/math_constants.h \
    GUI/hierarchymodel.h \
    GUI/hierarchyview.h \
    Shaders/colorshader.h \
    Shaders/textureshader.h \
    Shaders/phongshader.h \
    Shaders/shader.h \
    Components/component.h \
    Components/meshcomponent.h \
    Components/inputcomponent.h \
    Components/comppch.h \
    Systems/lightsystem.h \
    Systems/rendersystem.h \
    Systems/movementsystem.h \
    Views/renderview.h \
    Sound/soundmanager.h \
    Sound/soundsource.h \
    Sound/wavfilehandler.h \
    constants.h \
    billboard.h \
    gameobject.h \
    pool.h \
    renderwindow.h \
    resourcemanager.h \
    mainwindow.h \
    triangle.h \
    texture.h \
    vertex.h \
    camera.h \
    gltypes.h


SOURCES += main.cpp \
    Components/lightcomponent.cpp \
    Components/materialcomponent.cpp \
    Components/physicscomponent.cpp \
    Components/soundcomponent.cpp \
    Components/transformcomponent.cpp \
    GSL/matrix2x2.cpp \
    GSL/matrix3x3.cpp \
    GSL/matrix4x4.cpp \
    GSL/vector2d.cpp \
    GSL/vector3d.cpp \
    GSL/vector4d.cpp \
    GSL/gsl_math.cpp \
    GUI/hierarchymodel.cpp \
    GUI/hierarchyview.cpp \
    Shaders/colorshader.cpp \
    Shaders/textureshader.cpp \
    Shaders/phongshader.cpp \
    Shaders/shader.cpp \
    Components/component.cpp \
    Components/meshcomponent.cpp \
    Components/inputcomponent.cpp \
    Systems/lightsystem.cpp \
    Systems/rendersystem.cpp \
    Systems/movementsystem.cpp \
    Views/renderview.cpp \
    Sound/soundmanager.cpp \
    Sound/soundsource.cpp \
    Sound/wavfilehandler.cpp \
    billboard.cpp \
    gameobject.cpp \
    renderwindow.cpp \
    mainwindow.cpp \
    resourcemanager.cpp \
    triangle.cpp \
    texture.cpp \
    vertex.cpp \
    camera.cpp


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
