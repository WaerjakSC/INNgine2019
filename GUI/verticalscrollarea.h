#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H
#include <QScrollArea>

class QGroupBox;
class QVBoxLayout;
class VerticalScrollArea : public QScrollArea {
public:
    VerticalScrollArea(QWidget *parent = nullptr);

    void addGroupBox(QGroupBox *component);

    void clearLayout();

private:
    QVBoxLayout *componentListLayout;
    QWidget *contentWidget;
    void setupLayout();
};

#endif // VERTICALSCROLLAREA_H
