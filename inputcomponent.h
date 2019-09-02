#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include <QWindow>

class InputComponent : public QWindow
{
public:
    InputComponent();

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


private:
    void handleInput();

protected:

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

};


#endif // INPUTCOMPONENT_H
