#include "customspinbox.h"
#include <QStyleFactory>
#include <QWheelEvent>

CustomSpinBox::CustomSpinBox() {
    QStyle *fusion{QStyleFactory::create("fusion")};
    setMaximumWidth(58);
    setStyle(fusion);
    setFocusPolicy(Qt::StrongFocus);
}
