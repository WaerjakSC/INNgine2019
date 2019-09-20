#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H
#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>

class VerticalScrollArea : public QScrollArea {
public:
    VerticalScrollArea(QWidget *parent = nullptr);

    void addGroupBox(QGroupBox *component);

    void removeGroupBox(QGroupBox *component);

    void clearLayout();

private:
    QVBoxLayout *componentListLayout;
    QWidget *contentWidget;
    void setupLayout();
};

#endif // VERTICALSCROLLAREA_H
