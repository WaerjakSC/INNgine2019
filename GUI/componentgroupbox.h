#ifndef COMPONENTGROUPBOX_H
#define COMPONENTGROUPBOX_H

#include "components.h"
#include <QGroupBox>
class QMainWindow;
class MainWindow;
class ComponentGroupBox : public QGroupBox {
public:
    ComponentGroupBox(const QString &name, QWidget *parent = nullptr);
private slots:
    void removeComponent();

private:
    void createActions();
};

#endif // COMPONENTGROUPBOX_H
