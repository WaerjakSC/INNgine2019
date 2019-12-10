#ifndef COMPONENTGROUPBOX_H
#define COMPONENTGROUPBOX_H

#include <QGroupBox>

class ComponentGroupBox : public QGroupBox {
public:
    ComponentGroupBox(const QString &name, QWidget *parent = nullptr);
private slots:
    void removeComponent();

private:
    void createActions();
};

#endif // COMPONENTGROUPBOX_H
