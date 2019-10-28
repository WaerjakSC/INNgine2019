#ifndef COMPONENTGROUPBOX_H
#define COMPONENTGROUPBOX_H

#include "components.h"
#include <QGroupBox>
class QMainWindow;
class ComponentGroupBox : public QGroupBox {
public:
    ComponentGroupBox(CType type, QMainWindow *parent = nullptr);
private slots:
    void removeComponent();

private:
    void setTitleFromType();
    CType mComponentType;
    MainWindow *mMainWindow;
    void createActions();
};

#endif // COMPONENTGROUPBOX_H
