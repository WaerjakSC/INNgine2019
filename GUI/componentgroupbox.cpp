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
    if (title() == "Transform")
        registry->remove<Transform>(entityID);
    if (title() == "Material")
        registry->remove<Material>(entityID);
    if (title() == "Mesh")
        registry->remove<Mesh>(entityID);
    if (title() == "Light")
        registry->remove<Light>(entityID);
    if (title() == "Input")
        registry->remove<Input>(entityID);
    if (title() == "Physics")
        registry->remove<Physics>(entityID);
    if (title() == "Sound")
        registry->remove<Sound>(entityID);
    if (title() == "AI")
        registry->remove<AIcomponent>(entityID);
    if (title() == "AABB Collider")
        registry->remove<AABB>(entityID);
    if (title() == "OBB Collider")
        registry->remove<OBB>(entityID);
    if (title() == "Plane Collider")
        registry->remove<Plane>(entityID);
    if (title() == "Sphere Collider")
        registry->remove<Sphere>(entityID);
    if (title() == "Cylinder Collider")
        registry->remove<Cylinder>(entityID);
    deleteLater();
}

void ComponentGroupBox::createActions() {
    QString removeString = "Remove " + title() + " Component";
    QAction *remove = new QAction(tr(removeString.toStdString().c_str()));
    connect(remove, &QAction::triggered, this, &ComponentGroupBox::removeComponent);
    addAction(remove);
}
