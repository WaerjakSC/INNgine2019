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
        registry->removeComponent<Transform>(entityID);
    if (title() == "Material")
        registry->removeComponent<Material>(entityID);
    if (title() == "Mesh")
        registry->removeComponent<Mesh>(entityID);
    if (title() == "Light")
        registry->removeComponent<Light>(entityID);
    if (title() == "Input")
        registry->removeComponent<Input>(entityID);
    if (title() == "Physics")
        registry->removeComponent<Physics>(entityID);
    if (title() == "Sound")
        registry->removeComponent<Sound>(entityID);
    if (title() == "AI")
        registry->removeComponent<AIcomponent>(entityID);
    if (title() == "AABB Collider")
        registry->removeComponent<AABB>(entityID);
    if (title() == "OBB Collider")
        registry->removeComponent<OBB>(entityID);
    if (title() == "Plane Collider")
        registry->removeComponent<Plane>(entityID);
    if (title() == "Sphere Collider")
        registry->removeComponent<Sphere>(entityID);
    if (title() == "Cylinder Collider")
        registry->removeComponent<Cylinder>(entityID);
    deleteLater();
}

void ComponentGroupBox::createActions() {
    QString removeString = "Remove " + title() + " Component";
    QAction *remove = new QAction(tr(removeString.toStdString().c_str()));
    connect(remove, &QAction::triggered, this, &ComponentGroupBox::removeComponent);
    addAction(remove);
}
