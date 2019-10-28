#include "componentgroupbox.h"
#include "componentlist.h"
#include "entity.h"
#include "mainwindow.h"
#include "registry.h"
#include <QAction>
#include <QMainWindow>
#include <QStyleFactory>
ComponentGroupBox::ComponentGroupBox(CType type, QMainWindow *parent) : QGroupBox(parent), mComponentType(type) {
    setTitleFromType();
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
    switch (mComponentType) {
    case CType::Transform:
        registry->removeComponent<Transform>(entityID);
        break;
    case CType::Material:
        registry->removeComponent<Material>(entityID);
        break;
    case CType::Mesh:
        registry->removeComponent<Mesh>(entityID);
        break;
    case CType::Light:
        registry->removeComponent<Light>(entityID);
        break;
    case CType::Input:
        registry->removeComponent<Input>(entityID);
        break;
    case CType::Physics:
        registry->removeComponent<Physics>(entityID);
        break;
    case CType::Sound:
        registry->removeComponent<Sound>(entityID);
        break;
    case CType::Collision:
        registry->removeComponent<Collision>(entityID);
        break;
    default:
        break;
    }
    deleteLater();
}

void ComponentGroupBox::setTitleFromType() {
    switch (mComponentType) {
    case CType::Transform:
        setTitle("Transform");
        break;
    case CType::Material:
        setTitle("Material");
        break;
    case CType::Mesh:
        setTitle("Mesh");
        break;
    case CType::Light:
        setTitle("Light");
        break;
    case CType::Input:
        setTitle("Input");
        break;
    case CType::Physics:
        setTitle("Physics");
        break;
    case CType::Sound:
        setTitle("Sound");
        break;
    case CType::Collision:
        setTitle("Collision");
        break;
    default:
        break;
    }
}

void ComponentGroupBox::createActions() {
    QString removeString = "Remove " + title() + " Component";
    QAction *remove = new QAction(tr(removeString.toStdString().c_str()));
    connect(remove, &QAction::triggered, this, &ComponentGroupBox::removeComponent);
    addAction(remove);
}
