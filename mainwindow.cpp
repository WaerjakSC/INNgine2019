#include "mainwindow.h"
#include "hierarchymodel.h"
#include "innpch.h"
#include "movementsystem.h"
#include "registry.h"
#include "rendersystem.h"
#include "renderwindow.h"
#include "ui_mainwindow.h"
#include "verticalscrollarea.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QToolButton>
#include <componentlist.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow() {
    delete mRenderWindow;
    delete ui;
}

void MainWindow::init() {
    hierarchy = new HierarchyModel();
    hView = ui->SceneHierarchy;
    hView->setModel(hierarchy);
    hView->setMainWindow(this);
    scrollArea = new VerticalScrollArea();
    ui->horizontalTopLayout->addWidget(scrollArea);
    mComponentList = new ComponentList(this, scrollArea);

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
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mRenderWindowContainer->setSizePolicy(sizePolicy);
    mRenderWindowContainer->setMinimumSize(QSize(200, 200));
    //OpenGLLayout is made in the .ui-file!
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);
    playButtons();

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    mRenderWindowContainer->setFocus();

    //Set size of program in % of available screen
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    connect(hierarchy, &HierarchyModel::dataChanged, this, &MainWindow::onNameChanged);
    connect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
    connect(hView, &HierarchyView::dragSelection, this, &MainWindow::onEntityDragged);
    connect(hView, &HierarchyView::clicked, this, &MainWindow::onEntityClicked);
    connect(mRenderWindow, &RenderWindow::snapSignal, this, &MainWindow::snapToObject);
    connect(mRenderWindow, &RenderWindow::rayHitEntity, this, &MainWindow::mouseRayHit);
    connect(Registry::instance(), &Registry::entityCreated, this, &MainWindow::onEntityAdded);
    connect(Registry::instance(), &Registry::entityRemoved, hierarchy, &HierarchyModel::removeEntity);
    connect(Registry::instance(), &Registry::parentChanged, this, &MainWindow::parentChanged);
}
void MainWindow::playButtons() {
    QToolBar *toolbar = ui->mainToolBar;
    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->setMargin(0);
    play = new QToolButton;
    play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    play->setToolTip("Play");
    connect(play, &QToolButton::clicked, mRenderWindow, &RenderWindow::play);
    buttons->addWidget(play);

    pause = new QToolButton;
    pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    pause->setEnabled(false);
    pause->setToolTip("Pause");
    connect(pause, &QToolButton::clicked, mRenderWindow, &RenderWindow::pause);
    buttons->addWidget(pause);

    stop = new QToolButton;
    stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stop->setEnabled(false);
    stop->setToolTip("Stop");
    connect(stop, &QToolButton::clicked, mRenderWindow, &RenderWindow::stop);
    buttons->addWidget(stop);

    QGroupBox *box = new QGroupBox;
    box->setLayout(buttons);

    QWidget *spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer1->setVisible(true);
    toolbar->addWidget(spacer1); // Spacer #1

    toolbar->addWidget(box);

    QWidget *spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer2->setVisible(true);
    toolbar->addWidget(spacer2); // Spacer #2
}

void MainWindow::snapToObject() {
    if (selectedEntity)
        mRenderWindow->snapToObject(selectedEntity->id());
}
void MainWindow::createActions() {
    QMenu *projectActions = ui->menuBar->addMenu(tr("Project"));
    QAction *saveScene = new QAction(tr("Save"));
    projectActions->addAction(saveScene);
    connect(saveScene, &QAction::triggered, mRenderWindow, &RenderWindow::save);
    QAction *saveAs = new QAction(tr("Save As"));
    projectActions->addAction(saveAs);
    connect(saveAs, &QAction::triggered, mRenderWindow, &RenderWindow::saveAs);
    QAction *loadScene = new QAction(tr("Load"));
    projectActions->addAction(loadScene);
    connect(loadScene, &QAction::triggered, mRenderWindow, &RenderWindow::load);

    QAction *saveProject = new QAction(tr("Save Project"));
    projectActions->addAction(saveProject);
    connect(saveProject, &QAction::triggered, mRenderWindow, &RenderWindow::saveProject);
    QAction *loadProject = new QAction(tr("Open Project"));
    projectActions->addAction(loadProject);
    connect(loadProject, &QAction::triggered, mRenderWindow, &RenderWindow::loadProject);

    QAction *exit = new QAction(tr("Exit"));
    projectActions->addAction(exit);
    connect(exit, &QAction::triggered, this, &MainWindow::closeEngine);

    QMenu *gameObject = ui->menuBar->addMenu(tr("GameObject"));
    QAction *empty = new QAction(tr("Empty GameObject"), this);
    gameObject->addAction(empty);
    QMenu *make3D = gameObject->addMenu(tr("3D Object"));
    QAction *cube = new QAction(tr("Cube"), this);
    make3D->addAction(cube);
    QAction *sphere = new QAction(tr("Sphere"), this);
    make3D->addAction(sphere);
    QAction *plane = new QAction(tr("Plane"), this);
    make3D->addAction(plane);

    QMenu *components = ui->menuBar->addMenu(tr("Add Components")); // Maybe disable specific component if selected entity has that component already
    QAction *transform = new QAction(tr("Transform"), this);
    components->addAction(transform);
    connect(transform, &QAction::triggered, mComponentList, &ComponentList::addTransformComponent);

    QAction *material = new QAction(tr("Material"), this);
    components->addAction(material);
    connect(material, &QAction::triggered, mComponentList, &ComponentList::addMaterialComponent);

    QAction *mesh = new QAction(tr("Mesh"), this); // Somewhere along this action path should let you choose the mesh file to use/import
    components->addAction(mesh);
    connect(mesh, &QAction::triggered, mComponentList, &ComponentList::addMeshComponent);

    QAction *light = new QAction(tr("Light"), this);
    components->addAction(light);
    connect(light, &QAction::triggered, mComponentList, &ComponentList::addLightComponent);

    QAction *input = new QAction(tr("Input"), this);
    components->addAction(input);
    connect(input, &QAction::triggered, mComponentList, &ComponentList::addInputComponent);

    QAction *physics = new QAction(tr("Physics"), this);
    components->addAction(physics);
    connect(physics, &QAction::triggered, mComponentList, &ComponentList::addPhysicsComponent);

    QAction *sound = new QAction(tr("Sound"), this);
    components->addAction(sound);
    connect(sound, &QAction::triggered, mComponentList, &ComponentList::addSoundComponent);

    ui->mainToolBar->setMovable(false);

    // Set up the prefab actions
    connect(cube, &QAction::triggered, this, &MainWindow::makeCube);
    connect(sphere, &QAction::triggered, this, &MainWindow::makeSphere);
    connect(plane, &QAction::triggered, this, &MainWindow::makePlane);
    connect(empty, &QAction::triggered, this, &MainWindow::makeEntity);

    connect(this, &MainWindow::made3DObject, this, &MainWindow::onEntityAdded);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mRenderWindow->keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mRenderWindow->keyReleaseEvent(event);
}

void MainWindow::clearEditor() {
    hierarchy->clear();
    scrollArea->clearLayout();
}

void MainWindow::closeEngine() {
    ResourceManager::instance()->onExit();
    close();
}
void MainWindow::makeEntity() {
    emit made3DObject(Registry::instance()->makeEntity("GameObject"));
}
void MainWindow::makePlane() {
    emit made3DObject(ResourceManager::instance()->makePlane());
}
void MainWindow::makeSphere() {
    emit made3DObject(ResourceManager::instance()->makeOctBall());
}
void MainWindow::makeCube() {
    emit made3DObject(ResourceManager::instance()->makeCube());
}
void MainWindow::onParentChanged(const QModelIndex &newParent) {
    QString data = hierarchy->data(newParent).toString();
    Registry *registry = Registry::instance();
    if (data != "") {
        Entity *entt = registry->getEntity(data);
        if (entt) {
            // Undefined behavior if the dragged-to item doesn't have transform component (remember to add a parentChanged signal when transform component is removed from something I guess?)
            // Really not sure about this whole "Transform component governs parent/child relationship thing"
            if (registry->contains(entt->id(), CType::Transform)) {
                int parentID = entt->id();
                // Find entity in registry and set parentID to that object's ID, then get its transformcomponent and add the childID to its list of children.
                registry->setParent(selectedEntity->id(), parentID, true);
                mRenderWindow->movement()->updateEntity(selectedEntity->id());
            }
        }
    } else if (selectedEntity)
        if (registry->contains(selectedEntity->id(), CType::Transform)) {
            registry->setParent(selectedEntity->id(), -1, true);
            mRenderWindow->movement()->updateEntity(selectedEntity->id());
        }
}
void MainWindow::parentChanged(GLuint eID) {
    disconnect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
    QStandardItem *rootItem = hierarchy->invisibleRootItem();
    QStandardItem *item = hierarchy->itemFromEntityID(eID);
    if (item) {
        hierarchy->removeRow(item->row());
        Entity *entt = Registry::instance()->getEntity(eID);
        item = new QStandardItem;
        item->setText(entt->name());
        int parentID = Registry::instance()->getComponent<Transform>(entt->id()).parentID;
        if (Registry::instance()->hasParent(eID)) {
            QStandardItem *parent = hierarchy->itemFromEntityID(parentID);
            parent->insertRow(parent->rowCount(), item);
        } else
            rootItem->appendRow(item);
        mRenderWindow->movement()->updateEntity(eID);
    }
    connect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
}
void MainWindow::onEntityClicked(const QModelIndex &index) {
    Registry *reg = Registry::instance();
    QStandardItem *item = hierarchy->itemFromIndex(index);
    Entity *entt = reg->getEntity(item->text());
    onEntityDragged(entt->id());

    mComponentList->setupComponentList();
}

void MainWindow::onEntityDragged(GLuint eID) {
    selectedEntity = Registry::instance()->getEntity(eID);
}
void MainWindow::mouseRayHit(GLuint eID) {
    QStandardItem *entity = hierarchy->itemFromEntityID(eID);
    QModelIndex entityIndex = hierarchy->indexFromItem(entity);
    hView->setCurrentIndex(entityIndex);
    selectedEntity = Registry::instance()->getEntity(eID);
    mComponentList->setupComponentList();
}
void MainWindow::onNameChanged(const QModelIndex &index) {
    QString newName = hierarchy->data(index).toString();
    if (selectedEntity && Registry::instance()->isUniqueName(newName))
        selectedEntity->setName(hierarchy->data(index).toString());
}
void MainWindow::onEntityAdded(GLuint eID) {
    QStandardItem *parentItem = hierarchy->invisibleRootItem();
    QStandardItem *item = new QStandardItem;
    item->setText(Registry::instance()->getEntity(eID)->name());
    parentItem->appendRow(item);
}

/**
 * @brief Initial insertion of gameobjects, such as those made in an init function or read from a level file.
 * @param entities
 */
void MainWindow::insertEntities() {
    hierarchy->clear();
    QStandardItem *parentItem = hierarchy->invisibleRootItem();
    for (auto entity : Registry::instance()->getEntities()) {
        QStandardItem *item = new QStandardItem;
        if (entity.second->name() == "")
            item->setText(QString("GameObject" + QString::number(unnamedEntityCount)));
        else
            item->setText(entity.second->name());
        int parentID = Registry::instance()->getComponent<Transform>(entity.second->id()).parentID;
        if (parentID != -1) {
            forEach(parentID, item);
        } else
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
void MainWindow::forEach(GLuint parentID, QStandardItem *child, QModelIndex parent) {
    for (int r = 0; r < hierarchy->rowCount(parent); ++r) {
        QModelIndex index = hierarchy->index(r, 0, parent);
        QStandardItem *item = hierarchy->itemFromIndex(index);
        Entity *entt = Registry::instance()->getEntity(item->text());
        item->setText(entt->name());
        if (parentID == entt->id()) { // If in-value parentID matches the ID of the current item, append the new child to that item.
            item->appendRow(child);
            break;
        }
        if (hierarchy->hasChildren(index)) {
            forEach(parentID, child, index);
        }
    }
}
