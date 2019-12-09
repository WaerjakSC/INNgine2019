#include "mainwindow.h"
#include "hierarchymodel.h"
#include "innpch.h"
#include "inputsystem.h"
#include "movementsystem.h"
#include "registry.h"
#include "rendersystem.h"
#include "renderwindow.h"
#include "ui_mainwindow.h"
#include "verticalscrollarea.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QScreen>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QToolButton>
#include <componentlist.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent}, ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete mRenderWindow;
    delete ui;
}

void MainWindow::init()
{
    hierarchy = new HierarchyModel();
    hView = ui->SceneHierarchy;
    hView->setModel(hierarchy);
    hView->setMainWindow(this);
    scrollArea = new VerticalScrollArea();
    ui->horizontalTopLayout->addWidget(scrollArea);
    mComponentList = new ComponentList(scrollArea);

    //This will contain the setup of the OpenGL surface we will render into
    QSurfaceFormat format;

    //OpenGL v 4.1 - (Ole Flatens Mac does not support higher than this...)
    //you can try other versions, but then have to update RenderWindow and Shader
    //to inherit from other than QOpenGLFunctions_4_1_Core
    format.setVersion(4, 1);
    //Using the main profile for OpenGL - no legacy code permitted
    format.setProfile(QSurfaceFormat::CoreProfile);
    //A QSurface can be other types that OpenGL
    format.setRenderableType(QSurfaceFormat::OpenGL);

    //This should activate OpenGL debug Context used in RenderWindow::startOpenGLDebugger().
    //This line (and the startOpenGLDebugger() and checkForGLerrors() in RenderWindow class)
    //can be deleted, but it is nice to have OpenGL debug info!
    format.setOption(QSurfaceFormat::DebugContext);

    // The renderer will need a depth buffer - (not requiered to set in glfw-tutorials)
    format.setDepthBufferSize(24);

    //Set the number of samples used for multisampling
    format.setSamples(8);

    //Turn off VSync. If this is set to 1, VSync is on - default behaviour
    format.setSwapInterval(0);

    //Just prints out what OpenGL format we will get
    // - this can be deleted
    qDebug() << "Requesting surface format: " << format;

    //We have a format for the OpenGL window, so let's make it:
    mRenderWindow = new RenderWindow(format, this);
    registry = Registry::instance();
    createActions();

    //Check if renderwindow did initialize, else prints error and quit
    if (!mRenderWindow->context()) {
        qDebug() << "Failed to create context. Can not continue. Quits application!";
        delete mRenderWindow;
        return;
    }

    //The OpenGL RenderWindow got made, so continuing the setup:
    //We put the RenderWindow inside a QWidget so we can put in into a
    //layout that is made in the .ui-file
    mRenderWindowContainer = QWidget::createWindowContainer(mRenderWindow);
    QSizePolicy sizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding};
    mRenderWindowContainer->setSizePolicy(sizePolicy);
    mRenderWindowContainer->setMinimumSize(QSize(200, 200));
    //OpenGLLayout is made in the .ui-file!
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);
    playButtons();

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    mRenderWindowContainer->setFocus();

    //Set size of program in % of available screen
    resize(QGuiApplication::primaryScreen()->size() * 0.7);

    connect(hierarchy, &HierarchyModel::dataChanged, this, &MainWindow::onNameChanged);
    connect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
    connect(hView, &HierarchyView::clicked, this, &MainWindow::onEntityClicked);

    connect(this, &MainWindow::selectedEntity, registry, &Registry::setSelectedEntity);
    connect(registry, &Registry::entityCreated, this, &MainWindow::onEntityAdded);
    connect(registry, &Registry::entityRemoved, hierarchy, &HierarchyModel::removeEntity);
    connect(registry, &Registry::parentChanged, this, &MainWindow::parentChanged);
}
void MainWindow::playButtons()
{
    QToolBar *toolbar{ui->mainToolBar};
    QHBoxLayout *buttons{new QHBoxLayout};
    ResourceManager *factory{ResourceManager::instance()};
    buttons->setMargin(0);
    QToolButton *play{new QToolButton};
    play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    play->setToolTip("Play");
    connect(play, &QToolButton::clicked, factory, &ResourceManager::play);
    connect(factory, &ResourceManager::disablePlay, play, &QToolButton::setDisabled);

    buttons->addWidget(play);

    QToolButton *pause{new QToolButton};
    pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    pause->setEnabled(false);
    pause->setToolTip("Pause");
    connect(pause, &QToolButton::clicked, factory, &ResourceManager::pause);
    connect(factory, &ResourceManager::disablePause, pause, &QToolButton::setDisabled);

    buttons->addWidget(pause);

    QToolButton *stop{new QToolButton};
    stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stop->setEnabled(false);
    stop->setToolTip("Stop");
    connect(factory, &ResourceManager::disableStop, stop, &QToolButton::setDisabled);
    connect(stop, &QToolButton::clicked, factory, &ResourceManager::stop);
    buttons->addWidget(stop);

    QGroupBox *box{new QGroupBox};
    box->setLayout(buttons);

    QWidget *spacer1{new QWidget(this)};
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer1->setVisible(true);
    toolbar->addWidget(spacer1); // Spacer #1

    toolbar->addWidget(box);

    QWidget *spacer2{new QWidget(this)};
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer2->setVisible(true);
    toolbar->addWidget(spacer2); // Spacer #2
}

void MainWindow::createActions()
{
    QMenu *projectActions{ui->menuBar->addMenu(tr("&File"))};
    ResourceManager *factory{ResourceManager::instance()};
    std::vector<QAction *> menuActions;
    QAction *newScene{new QAction(tr("New Scene"))};
    projectActions->addAction(newScene);
    menuActions.push_back(newScene);
    connect(newScene, SIGNAL(triggered()), factory, SLOT(newScene()));
    QAction *saveScene{new QAction(tr("&Save"))};
    menuActions.push_back(saveScene);
    projectActions->addAction(saveScene);
    connect(saveScene, &QAction::triggered, factory, &ResourceManager::save);
    QAction *saveAs{new QAction(tr("Save &As"))};
    menuActions.push_back(saveAs);
    projectActions->addAction(saveAs);
    connect(saveAs, &QAction::triggered, factory, &ResourceManager::saveAs);
    QAction *loadScene{new QAction(tr("&Load"))};
    menuActions.push_back(loadScene);
    projectActions->addAction(loadScene);
    connect(loadScene, &QAction::triggered, factory, &ResourceManager::load);
    QAction *newProject{new QAction(tr("&New Project"))};
    menuActions.push_back(newProject);
    projectActions->addAction(newProject);
    connect(newProject, &QAction::triggered, factory, &ResourceManager::newProject);
    QAction *saveProject{new QAction(tr("Save &Project"))};
    menuActions.push_back(saveProject);
    projectActions->addAction(saveProject);
    connect(saveProject, &QAction::triggered, factory, &ResourceManager::saveProject);
    QAction *loadProject{new QAction(tr("&Open Project"))};
    menuActions.push_back(loadProject);
    projectActions->addAction(loadProject);
    connect(loadProject, SIGNAL(triggered()), factory, SLOT(loadProject()));

    for (auto action : menuActions) {
        connect(factory, &ResourceManager::disableActions, action, &QAction::setDisabled);
    }

    QAction *exit{new QAction(tr("&Exit"))};
    projectActions->addAction(exit);
    connect(exit, &QAction::triggered, this, &MainWindow::closeEngine);

    QMenu *editor{ui->menuBar->addMenu(tr("&Editor"))};
    QAction *wireframe{new QAction(tr("&Wireframe"), this)};
    wireframe->setCheckable(true);
    connect(wireframe, &QAction::triggered, mRenderWindow, &RenderWindow::toggleWireframe);
    editor->addAction(wireframe);
    QAction *xyz{new QAction(tr("&XYZ Lines"), this)};
    xyz->setCheckable(true);
    xyz->setChecked(true);
    connect(xyz, &QAction::triggered, mRenderWindow, &RenderWindow::toggleXYZ);
    editor->addAction(xyz);
    QAction *dragDrop{new QAction(tr("&Debug Drag'n'Drop"))};
    dragDrop->setCheckable(true);
    connect(dragDrop, &QAction::triggered, mRenderWindow, &RenderWindow::togglePlaneDebugMode);
    editor->addAction(dragDrop);

    QMenu *entity{ui->menuBar->addMenu(tr("&Entity"))};
    QAction *empty{new QAction(tr("Empty &Entity"), this)};
    entity->addAction(empty);
    QMenu *make3D{entity->addMenu(tr("3D &Object"))};
    QAction *cube{new QAction(tr("&Cube"), this)};
    make3D->addAction(cube);
    QAction *sphere{new QAction(tr("&Sphere"), this)};
    make3D->addAction(sphere);
    QAction *plane{new QAction(tr("&Plane"), this)};
    make3D->addAction(plane);

    QMenu *components{ui->menuBar->addMenu(tr("Add &Components"))}; // Maybe disable specific component if selected entity has that component already
    QAction *transAction{new QAction(tr("Transform"), this)};
    components->addAction(transAction);
    connect(transAction, &QAction::triggered, mComponentList, &ComponentList::addTransformComponent);

    QAction *matAction{new QAction(tr("Material"), this)};
    components->addAction(matAction);
    connect(matAction, &QAction::triggered, mComponentList, &ComponentList::addMaterialComponent);

    QAction *meshAction{new QAction(tr("Mesh"), this)};
    components->addAction(meshAction);
    connect(meshAction, &QAction::triggered, mComponentList, &ComponentList::addMeshComponent);

    QMenu *collisionMenu{components->addMenu(tr("&Colliders"))};

    QAction *AABBAction{new QAction(tr("AABB"), this)};
    collisionMenu->addAction(AABBAction);
    connect(AABBAction, &QAction::triggered, mComponentList, &ComponentList::addAABBCollider);

    QAction *SphereAction{new QAction(tr("Sphere"), this)};
    collisionMenu->addAction(SphereAction);
    connect(SphereAction, &QAction::triggered, mComponentList, &ComponentList::addSphereCollider);

    QAction *lightAction{new QAction(tr("Light"), this)};
    components->addAction(lightAction);
    connect(lightAction, &QAction::triggered, mComponentList, &ComponentList::addLightComponent);

    QAction *physicsAction{new QAction(tr("Physics"), this)};
    components->addAction(physicsAction);
    connect(physicsAction, &QAction::triggered, mComponentList, &ComponentList::addPhysicsComponent);

    QAction *soundAction{new QAction(tr("Sound"), this)};
    components->addAction(soundAction);
    connect(soundAction, &QAction::triggered, mComponentList, &ComponentList::addSoundComponent);

    QAction *emitterAction{new QAction(tr("Particle Emitter"), this)};
    components->addAction(emitterAction);
    connect(emitterAction, &QAction::triggered, mComponentList, &ComponentList::addParticleEmitter);

    QAction *aiAction{new QAction(tr("AI"), this)};
    components->addAction(aiAction);
    connect(aiAction, &QAction::triggered, mComponentList, &ComponentList::addAIComponent);

    QAction *bspline{new QAction(tr("BSplinePoint"), this)};
    components->addAction(bspline);
    connect(bspline, &QAction::triggered, mComponentList, &ComponentList::addBSplineComponent);

    QAction *buildable{new QAction(tr("Buildable"), this)};
    components->addAction(buildable);
    connect(buildable, &QAction::triggered, mComponentList, &ComponentList::addBuildableComponent);

    QAction *cam{new QAction(tr("Game Camera"), this)};
    components->addAction(cam);
    connect(cam, &QAction::triggered, mComponentList, &ComponentList::addGameCameraComponent);

    ui->mainToolBar->setMovable(false);

    // Set up the prefab actions
    connect(cube, &QAction::triggered, this, &MainWindow::makeCube);
    connect(sphere, &QAction::triggered, this, &MainWindow::makeSphere);
    connect(plane, &QAction::triggered, this, &MainWindow::makePlane);
    connect(empty, &QAction::triggered, this, &MainWindow::makeEntity);

    connect(this, &MainWindow::made3DObject, this, &MainWindow::onEntityAdded);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F)
        mRenderWindow->keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F)
        mRenderWindow->keyReleaseEvent(event);
}

void MainWindow::clearEditor()
{
    hierarchy->clear();
    scrollArea->clearLayout();
    insertEntities();
}

void MainWindow::closeEngine()
{
    ResourceManager::instance()->onExit();
    close();
}
void MainWindow::makeEntity()
{
    emit made3DObject(registry->makeEntity("Entity", false));
}
void MainWindow::makePlane()
{
    emit made3DObject(ResourceManager::instance()->makePlane());
}
void MainWindow::makeSphere()
{
    emit made3DObject(ResourceManager::instance()->makeOctBall());
}
void MainWindow::makeCube()
{
    emit made3DObject(ResourceManager::instance()->makeCube());
}
void MainWindow::onParentChanged(const QModelIndex &newParent)
{
    int data{hierarchy->data(newParent, 257).toInt()};
    GLuint selectedEntity{registry->getSelectedEntity()};
    if (data >= 0) {
        GLuint parent{static_cast<GLuint>(data)};
        if (newParent.isValid()) {
            if (registry->contains<Transform>(parent) && selectedEntity) {
                // Find entity in registry and set parentID to that object's ID, then get its transformcomponent and add the childID to its list of children.
                registry->setParent(selectedEntity, parent, true);
                registry->system<MovementSystem>()->updateEntity(selectedEntity);
            }
        }
    }
    else if (selectedEntity)
        if (registry->contains<Transform>(selectedEntity)) {
            registry->setParent(selectedEntity, -1, true);
            registry->system<MovementSystem>()->updateEntity(selectedEntity);
        }
}
void MainWindow::parentChanged(GLuint eID)
{
    disconnect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
    QStandardItem *rootItem{hierarchy->invisibleRootItem()};
    QStandardItem *item{hierarchy->itemFromEntityID(eID)};
    if (item) {
        hierarchy->removeRow(item->row());
        auto &entt{registry->get<EInfo>(eID)};
        item = new QStandardItem;
        item->setText(entt.name);
        item->setData(eID);
        int parentID{registry->get<Transform>(eID).parentID};
        if (registry->hasParent(eID)) {
            QStandardItem *parent{hierarchy->itemFromEntityID(parentID)};
            parent->insertRow(parent->rowCount(), item);
        }
        else
            rootItem->appendRow(item);
        registry->system<MovementSystem>()->updateEntity(eID);
    }
    connect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
}
void MainWindow::onEntityClicked(const QModelIndex &index)
{
    GLuint eID{hierarchy->itemFromIndex(index)->data().toUInt()};
    emit selectedEntity(eID);
    mComponentList->setupComponentList();
}
void MainWindow::mouseRayHit(int eID)
{
    if (eID == -1) {
        hView->setCurrentIndex(hierarchy->invisibleRootItem()->index());
        scrollArea->clearLayout();
        return;
    }
    QStandardItem *entity{hierarchy->itemFromEntityID(eID)};
    hView->setCurrentIndex(hierarchy->indexFromItem(entity));
    registry->setSelectedEntity(eID);
    mComponentList->setupComponentList();
}
void MainWindow::onNameChanged(const QModelIndex &index)
{
    QString newName{hierarchy->data(index).toString()};
    GLuint selectedEntity{registry->getSelectedEntity()};
    auto &info{registry->get<EInfo>(selectedEntity)};
    if (selectedEntity)
        info.name = hierarchy->data(index).toString();
}
void MainWindow::onEntityAdded(GLuint eID)
{
    QStandardItem *parentItem{hierarchy->invisibleRootItem()};
    QStandardItem *item{new QStandardItem};
    auto &info{registry->get<EInfo>(eID)};
    item->setText(info.name);
    item->setData(eID);
    parentItem->appendRow(item);

    connect(registry, &Registry::nameChanged, this, &MainWindow::changeEntityName);
}

void MainWindow::onEntityRemoved(GLuint entity)
{
    QStandardItem *item{hierarchy->itemFromEntityID(entity)};
    hierarchy->removeRow(hierarchy->indexFromItem(item).row());
}
void MainWindow::changeEntityName(const GLuint &entt)
{
    QStandardItem *item{hierarchy->itemFromEntityID(entt)};
    auto &info{registry->get<EInfo>(entt)};
    item->setText(info.name);
}
/**
 * @brief Initial insertion of entities, such as those made in an init function or read from a level file.
 * @param entities
 */
void MainWindow::insertEntities()
{
    hierarchy->clear();
    QStandardItem *parentItem{hierarchy->invisibleRootItem()};
    for (auto entity : registry->getEntities()) {
        auto &info{registry->get<EInfo>(entity)};
        if (info.isDestroyed)
            continue;
        QStandardItem *item{new QStandardItem};
        if (info.name == "")
            item->setText(QString("Entity" + QString::number(unnamedEntityCount)));
        else
            item->setText(info.name);
        item->setData(entity);
        if (registry->contains<Transform>(entity)) {
            int parentID{registry->get<Transform>(entity).parentID};
            if (parentID != -1) {
                forEach(parentID, item);
            }
            else
                parentItem->appendRow(item);
        }
        else
            parentItem->appendRow(item);
    }
}

/**
 * @brief Iterate through a model to find a specific item
 * @param model
 * @param parentName
 * @param child
 * @param parent
 */
void MainWindow::forEach(GLuint parentID, QStandardItem *child, QModelIndex parent)
{
    for (int r = 0; r < hierarchy->rowCount(parent); ++r) {
        QModelIndex index{hierarchy->index(r, 0, parent)};
        QStandardItem *item{hierarchy->itemFromIndex(index)};
        if (parentID == item->data().toUInt()) { // If in-value parentID matches the ID of the current item, append the new child to that item.
            item->appendRow(child);
            break;
        }
        if (hierarchy->hasChildren(index)) {
            forEach(parentID, child, index);
        }
    }
}
