#include "componentgroupbox.h"
#include "componentlist.h"
#include "mainwindow.h"
#include "registry.h"
#include <QAction>
#include <QMainWindow>
#include <QStyleFactory>
ComponentGroupBox::ComponentGroupBox(const QString &name, QWidget *parent) : QGroupBox{parent} {
    setTitle(name);
    QStyle *fusion = QStyleFactory::create("fusion");
    setAlignment(Qt::AlignCenter);
    setStyle(fusion);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    createActions();
}

void ComponentGroupBox::removeComponent() {
    Registry *registry{Registry::instance()};
    GLuint entityID{registry->getSelectedEntity()};
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
        registry->remove<AIComponent>(entityID);
    if (title() == "AABB Collider")
        registry->remove<AABB>(entityID);
    if (title() == "Sphere Collider")
        registry->remove<Sphere>(entityID);
    if (title() == "Particle Emitter")
        registry->remove<ParticleEmitter>(entityID);
    if (title() == "Game Camera")
        registry->remove<GameCamera>(entityID);
    deleteLater();
}

void ComponentGroupBox::createActions() {
    QString removeString = "Remove " + title() + " Component";
    QAction *remove{new QAction(tr(removeString.toStdString().c_str()))};
    connect(remove, &QAction::triggered, this, &ComponentGroupBox::removeComponent);
    addAction(remove);
}
