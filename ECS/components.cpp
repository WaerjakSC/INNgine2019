#include "components.h"
#include "mainwindow.h"
#include "registry.h"

Component::~Component() {
}

/*
void Input::wheelEvent(QWheelEvent *event){
    QPoint numDegrees = event->angleDelta() / 8;
    RenderWindow r;
    //if RMB, change the speed of the camera
    if (RMB) {
        if (numDegrees.y() < 1)
        r.setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
        r.setCameraSpeed(-0.001f);
    }
    event->accept();
}
*/
