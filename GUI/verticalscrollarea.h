#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H
#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
namespace cjk {
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

} // namespace cjk

#endif // VERTICALSCROLLAREA_H
