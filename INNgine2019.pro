QT          += core gui widgets qml
TEMPLATE    = app
CONFIG      += c++17

TARGET      = INNgine2019

PRECOMPILED_HEADER = innpch.h

INCLUDEPATH += \
    ./GSL \
    ./ECS \
    ./ECS/Systems \
    ./ECS/Views \
    ./Shaders \
    ./Systems \
    ./Views \
    ./GUI \
    ./Resources \

mac {
    LIBS += -framework OpenAL
}

win32 {
    INCLUDEPATH += $(OPENAL_HOME)\\include\\AL

    #Visual Studio 64-bit
    contains(QT_ARCH, x86_64) {
        LIBS *= $(OPENAL_HOME)\\libs\\Win64\\OpenAL32.lib
        # Copy required DLLs to output directory
        CONFIG(debug, debug|release) {
            OpenAL32.commands = copy /Y \"$(OPENAL_HOME)\\bin\\Win64\\OpenAL32.dll\" debug
            OpenAL32.target = debug/OpenAL32.dll

            QMAKE_EXTRA_TARGETS += OpenAL32
            PRE_TARGETDEPS += debug/OpenAL32.dll
        } else:CONFIG(release, debug|release) {
            OpenAL32.commands = copy /Y \"$(OPENAL_HOME)\\bin\\Win64\\OpenAL32.dll\" release
            OpenAL32.target = release/OpenAL32.dll

            QMAKE_EXTRA_TARGETS += OpenAL32
            PRE_TARGETDEPS += release/OpenAL32.dll release/OpenAL32.dll
        } else {
            error(Unknown set of dependencies.)
        }
    } else {
        LIBS *= $(OPENAL_HOME)\\libs\\Win32\\OpenAL32.lib
        # Copy required DLLs to output directory
        CONFIG(debug, debug|release) {
            OpenAL32.commands = copy /Y \"$(OPENAL_HOME)\\bin\\Win32\\OpenAL32.dll\" debug
            OpenAL32.target = debug/OpenAL32.dll

            QMAKE_EXTRA_TARGETS += OpenAL32
            PRE_TARGETDEPS += debug/OpenAL32.dll
        } else:CONFIG(release, debug|release) {
            OpenAL32.commands = copy /Y \"$(OPENAL_HOME)\\bin\\Win32\\OpenAL32.dll\" release
            OpenAL32.target = release/OpenAL32.dll

            QMAKE_EXTRA_TARGETS += OpenAL32
            PRE_TARGETDEPS += release/OpenAL32.dll release/OpenAL32.dll
        } else {
            error(Unknown set of dependencies.)
        }
    }
}

HEADERS += \
    ECS/Systems/isystem.h \
    ECS/Systems/aisystem.h \
    ECS/Systems/inputsystem.h \
    ECS/Systems/scriptsystem.h \
    ECS/Systems/soundsystem.h \
    ECS/Systems/lightsystem.h \
    ECS/Systems/rendersystem.h \
    ECS/Systems/movementsystem.h \
    ECS/Systems/collisionsystem.h \
#
    ECS/entity.h \
    ECS/pool.h \
    ECS/view.h \
    ECS/components.h \
    ECS/registry.h \
#
    GSL/matrix2x2.h \
    GSL/matrix3x3.h \
    GSL/matrix4x4.h \
    GSL/vector2d.h \
    GSL/vector3d.h \
    GSL/vector4d.h \
    GSL/gsl_math.h \
    GSL/vertex.h \
    GSL/math_constants.h \
#
    GUI/componentgroupbox.h \
    GUI/componentlist.h \
    GUI/hierarchymodel.h \
    GUI/hierarchyview.h \
    GUI/verticalscrollarea.h \
#    
    Shaders/colorshader.h \
    Shaders/textureshader.h \
    Shaders/phongshader.h \
    Shaders/shader.h \
#
    Resources/scene.h \
    Resources/wavfilehandler.h \
    Resources/resourcemanager.h \
    Resources/texture.h \
    Resources/triangle.h \
    Resources/tiny_obj_loader.h \
#
    bsplinecurve.h \
    cameracontroller.h \
    constants.h \
    billboard.h \
    core.h \
    core.h \
    deltaTime.h \
    deltaTime.h \
    raycast.h \
    renderwindow.h \
    mainwindow.h \
    camera.h \
    gltypes.h


SOURCES += main.cpp \
    ECS/Systems/aisystem.cpp \
    ECS/Systems/inputsystem.cpp \
    ECS/Systems/scriptsystem.cpp \
    ECS/Systems/soundsystem.cpp \
    ECS/Systems/lightsystem.cpp \
    ECS/Systems/rendersystem.cpp \
    ECS/Systems/movementsystem.cpp \
    ECS/Systems/collisionsystem.cpp \
#
    ECS/entity.cpp \
    ECS/components.cpp \
    ECS/registry.cpp \
#
    GSL/matrix2x2.cpp \
    GSL/matrix3x3.cpp \
    GSL/matrix4x4.cpp \
    GSL/vector2d.cpp \
    GSL/vector3d.cpp \
    GSL/vector4d.cpp \
    GSL/vertex.cpp \
    GSL/gsl_math.cpp \
#
    GUI/componentgroupbox.cpp \
    GUI/componentlist.cpp \
    GUI/hierarchymodel.cpp \
    GUI/hierarchyview.cpp \
    GUI/verticalscrollarea.cpp \
#
    Shaders/colorshader.cpp \
    Shaders/textureshader.cpp \
    Shaders/phongshader.cpp \
    Shaders/shader.cpp \
#
    Resources/wavfilehandler.cpp \
    Resources/resourcemanager.cpp \
    Resources/texture.cpp \
    Resources/triangle.cpp \
    Resources/scene.cpp \
#
    billboard.cpp \
    bsplinecurve.cpp \
    cameracontroller.cpp \
    raycast.cpp \
    renderwindow.cpp \
    mainwindow.cpp \
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

OTHER_FILES += \
    Assets/Scripts/testscript.js

 RESOURCES += \
 icons.qrc
