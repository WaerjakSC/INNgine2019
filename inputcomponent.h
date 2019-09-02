#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H
#include <QKeyEvent>
#include <QMouseEvent>
#include "mainwindow.h"

class InputComponent {
public:
    InputComponent(MainWindow *mainWindow);

    bool W{false};
    bool A{false};
    bool S{false};
    bool D{false};
    bool UP{false}; //Arrow keys
    bool DOWN{false};
    bool LEFT{false};
    bool RIGHT{false};
    bool Q{false};
    bool E{false};
    bool C{false};
    bool LSHIFT{false};
    bool SPACE{false};
    bool LMB{false};
    bool RMB{false};
    bool MMB{false};
    float MWHEEL{0.f};
    int MOUSEX{0};
    int MOUSEY{0};

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void handleInput();
    MainWindow *mMainWindow;

protected:
};

#endif // INPUTCOMPONENT_H
