#ifndef CUSTOMDOUBLESPINBOX_H
#define CUSTOMDOUBLESPINBOX_H
#include <QDoubleSpinBox>

class CustomDoubleSpinBox : public QDoubleSpinBox {
public:
    CustomDoubleSpinBox(const std::optional<float> &minRange = std::nullopt, const std::optional<float> &maxRange = std::nullopt);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
};

#endif // CUSTOMDOUBLESPINBOX_H
