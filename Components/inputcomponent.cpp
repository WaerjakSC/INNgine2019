#include "inputcomponent.h"
#include "renderwindow.h"
#include <QKeyEvent>

// InputComponent handling user inputs (moved from renderwindow and implemented here)

// Updates each game loop/tick
void InputComponent::update(float dt)
{

}

// Code initialization (Similar to Unity's void Start()), separating the constructor and code initialization
void InputComponent::init()
{

}

InputComponent::InputComponent(MainWindow *mainWindow) : mMainWindow(mainWindow)
{

}

void InputComponent::keyPressEvent(QKeyEvent *event){
    //    You get the keyboard input like this

    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mMainWindow->close();
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

void InputComponent::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_W){
        W = false;
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

void InputComponent::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton)
        RMB = true;
    if (event->button() == Qt::LeftButton)
        LMB = true;
    if (event->button() == Qt::MiddleButton)
        MMB = true;
}

void InputComponent::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton)
        RMB = false;
    if (event->button() == Qt::LeftButton)
        LMB = false;
    if (event->button() == Qt::MiddleButton)
        MMB = false;
}

/*
void InputComponent::wheelEvent(QWheelEvent *event){
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


