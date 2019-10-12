#include <cmath>
#include "components.h"
#include "mainwindow.h"

Component::~Component() {
}

void Input::keyPressEvent(QKeyEvent *event) {
    //    You get the keyboard input like this

    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mMainWindow->close();
    }
    if (event->key() == Qt::Key_Control)
        CTRL = true;
    if (event->key() == Qt::Key_L) {
        L = true;
    }
    if (event->key() == Qt::Key_F) {
        F = true;
    }
    if (event->key() == Qt::Key_W) {
        W = true;
    }
    if (event->key() == Qt::Key_S) {
        S = true;
    }
    if (event->key() == Qt::Key_D) {
        D = true;
    }
    if (event->key() == Qt::Key_A) {
        A = true;
    }
    if (event->key() == Qt::Key_Q) {
        Q = true;
    }
    if (event->key() == Qt::Key_E) {
        E = true;
    }
    if (event->key() == Qt::Key_Z) {
    }
    if (event->key() == Qt::Key_X) {
    }
    if (event->key() == Qt::Key_Up) {
        UP = true;
    }
    if (event->key() == Qt::Key_Down) {
        DOWN = true;
    }
    if (event->key() == Qt::Key_Left) {
        LEFT = true;
    }
    if (event->key() == Qt::Key_Right) {
        RIGHT = true;
    }
    if (event->key() == Qt::Key_U) {
    }
    if (event->key() == Qt::Key_O) {
    }
}

void Input::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W) {
        W = false;
    }
    if (event->key() == Qt::Key_Control)
        CTRL = false;
    if (event->key() == Qt::Key_L) {
        L = false;
    }
    if (event->key() == Qt::Key_F) {
        F = false;
    }
    if (event->key() == Qt::Key_S) {
        S = false;
    }
    if (event->key() == Qt::Key_D) {
        D = false;
    }
    if (event->key() == Qt::Key_A) {
        A = false;
    }
    if (event->key() == Qt::Key_Q) {
        Q = false;
    }

    if (event->key() == Qt::Key_E) {
        E = false;
    }

    if (event->key() == Qt::Key_Z) {
    }

    if (event->key() == Qt::Key_X) {
    }

    if (event->key() == Qt::Key_Up) {
        UP = false;
    }

    if (event->key() == Qt::Key_Down) {
        DOWN = false;
    }

    if (event->key() == Qt::Key_Left) {
        LEFT = false;
    }

    if (event->key() == Qt::Key_Right) {
        RIGHT = false;
    }

    if (event->key() == Qt::Key_U) {
    }

    if (event->key() == Qt::Key_O) {
    }
}

void Input::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        RMB = true;
    if (event->button() == Qt::LeftButton)
        LMB = true;
    if (event->button() == Qt::MiddleButton)
        MMB = true;
}

void Input::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        RMB = false;
    if (event->button() == Qt::LeftButton)
        LMB = false;
    if (event->button() == Qt::MiddleButton)
        MMB = false;
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

vec3 Collision::getMin(const Collision::AABB &aabb)
{
    vec3 p1 = aabb.origin + aabb.size;
    vec3 p2 = aabb.origin - aabb.size;

    return vec3(fminf(p1.x, p2.x),
                fminf(p1.y, p2.y),
                fminf(p1.z, p2.z));
}

vec3 Collision::getMax(const Collision::AABB &aabb)
{
    vec3 p1 = aabb.origin + aabb.size;
    vec3 p2 = aabb.origin - aabb.size;

    return vec3(fmaxf(p1.x, p2.x),
                fmaxf(p1.y, p2.y),
                fmaxf(p1.z, p2.z));
}
