#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H
#include "component.h"
#include "mainwindow.h"
#include <QKeyEvent>
#include <QMouseEvent>

class InputComponent : public Component {
public:
    virtual void update(float dt);
    virtual void init();

    InputComponent();

    bool W{false};
    bool A{false};
    bool S{false};
    bool D{false};
    bool L{false};
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

    void setMainWindow(MainWindow *mainWindow);

private:
    void handleInput();
    MainWindow *mMainWindow;

protected:
};

#endif // INPUTCOMPONENT_H
