#include "verticalscrollarea.h"

VerticalScrollArea::VerticalScrollArea(QWidget *parent) : QScrollArea(parent) {
    setWidgetResizable(true);
    contentWidget = new QWidget(this);
    setWidget(contentWidget);
    setupLayout();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMaximumWidth(260);
}

void VerticalScrollArea::addGroupBox(QGroupBox *component) {
    componentListLayout->addWidget(component);
}
/**
 * @brief VerticalScrollArea::removeGroupBox
 * @todo implement once you can remove components from an entity
 * @param component
 */
void VerticalScrollArea::removeGroupBox(QGroupBox *component) {
}
/**
 * @brief Delete all the children in the Layout and re-make it with new content and connections.
 */
void VerticalScrollArea::clearLayout() {
    qDeleteAll(contentWidget->children());
    setupLayout();
}
void VerticalScrollArea::setupLayout() {
    componentListLayout = new QVBoxLayout(contentWidget);
    componentListLayout->setAlignment(Qt::AlignTop);
    componentListLayout->setMargin(2);
}
