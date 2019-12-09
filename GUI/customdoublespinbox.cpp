#include "customdoublespinbox.h"
#include <QStyleFactory>
#include <QWheelEvent>

CustomDoubleSpinBox::CustomDoubleSpinBox(const std::optional<float> &minRange, const std::optional<float> &maxRange)
{
    QStyle *fusion{QStyleFactory::create("fusion")};
    setDecimals(2);
    setRange(minRange.value_or(-5000), maxRange.value_or(5000));
    setMaximumWidth(58);
    setStyle(fusion);
    setFocusPolicy(Qt::StrongFocus);
}

void CustomDoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (!hasFocus()) {
        event->ignore();
    }
    else {
        QDoubleSpinBox::wheelEvent(event);
    }
}
