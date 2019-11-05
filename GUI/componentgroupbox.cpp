#include "componentgroupbox.h"
#include "componentlist.h"
#include "entity.h"
#include "mainwindow.h"
#include "registry.h"
#include <QAction>
#include <QMainWindow>
#include <QStyleFactory>
ComponentGroupBox::ComponentGroupBox(const QString &name, QMainWindow *parent) : QGroupBox(parent) {
    setTitle(name);
    mMainWindow = static_cast<MainWindow *>(parent);
    QStyle *fusion = QStyleFactory::create("fusion");
    setAlignment(Qt::AlignCenter);
    setStyle(fusion);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    createActions();
}

void ComponentGroupBox::removeComponent() {
    Registry *registry = Registry::instance();
    GLuint entityID = mMainWindow->selectedEntity->id();
    if (registry->contains<Transform>(entityID))
        registry->removeComponent<Transform>(entityID);
    if (registry->contains<Material>(entityID))
        registry->removeComponent<Material>(entityID);
    if (registry->contains<Mesh>(entityID))
        registry->removeComponent<Mesh>(entityID);
    if (registry->contains<Light>(entityID))
        registry->removeComponent<Light>(entityID);
    if (registry->contains<Input>(entityID))
        registry->removeComponent<Input>(entityID);
    if (registry->contains<Physics>(entityID))
        registry->removeComponent<Physics>(entityID);
    if (registry->contains<Sound>(entityID))
        registry->removeComponent<Sound>(entityID);
    if (registry->contains<AIcomponent>(entityID))
        registry->removeComponent<AIcomponent>(entityID);
    if (registry->contains<AABB>(entityID))
        registry->removeComponent<AABB>(entityID);
    if (registry->contains<OBB>(entityID))
        registry->removeComponent<OBB>(entityID);
    if (registry->contains<Plane>(entityID))
        registry->removeComponent<Plane>(entityID);
    if (registry->contains<Sphere>(entityID))
        registry->removeComponent<Sphere>(entityID);
    if (registry->contains<Cylinder>(entityID))
        registry->removeComponent<Cylinder>(entityID);
    deleteLater();
}

void ComponentGroupBox::createActions() {
    QString removeString = "Remove " + title() + " Component";
    QAction *remove = new QAction(tr(removeString.toStdString().c_str()));
    connect(remove, &QAction::triggered, this, &ComponentGroupBox::removeComponent);
    addAction(remove);
}
