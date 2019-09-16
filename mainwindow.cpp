#include "mainwindow.h"
#include "GUI/hierarchymodel.h"
#include "Systems/rendersystem.h"
#include "innpch.h"
#include "renderwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QDesktopWidget>
#include <QStandardItem>
#include <QSurfaceFormat>

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
    ui->SceneHierarchy->setModel(hierarchy);
    hView = ui->SceneHierarchy;

    createActions();

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

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    mRenderWindowContainer->setFocus();

    //Set size of program in % of available screen
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    connect(hierarchy, &HierarchyModel::dataChanged, this, &MainWindow::onNameChanged);
    connect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
    connect(hView, &HierarchyView::dragSelection, this, &MainWindow::onGameObjectDragged);
    connect(hView, &QTreeView::clicked, this, &MainWindow::onGameObjectClicked);
}
void MainWindow::createActions() {
    QMenu *gameObject = ui->menuBar->addMenu(tr("&GameObject"));
    QMenu *make3D = gameObject->addMenu(tr("3D Object"));
    QAction *cube = new QAction(tr("Cube"), this);
    make3D->addAction(cube);
    QAction *sphere = new QAction(tr("Sphere"), this);
    make3D->addAction(sphere);
    QAction *plane = new QAction(tr("Plane"), this);
    make3D->addAction(plane);
    //    ui->mainToolBar->addWidget(objectCreation);
    ui->mainToolBar->setMovable(false);
    connect(cube, &QAction::triggered, this, &MainWindow::makeCube);
    connect(sphere, &QAction::triggered, this, &MainWindow::makeSphere);
    connect(plane, &QAction::triggered, this, &MainWindow::makePlane);
    connect(this, &MainWindow::made3DObject, this, &MainWindow::onGameObjectsChanged);
}
void MainWindow::makePlane() {
    emit made3DObject(mRenderWindow->factory().makePlane());
}
void MainWindow::makeSphere() {
    emit made3DObject(mRenderWindow->factory().makeOctBall());
}
void MainWindow::makeCube() {
    emit made3DObject(mRenderWindow->factory().makeCube());
}
void MainWindow::onParentChanged(const QModelIndex &parent) {
    QString data = hierarchy->data(parent).toString();
    if (data != "") {
        int parentID;
        for (auto entity : mRenderWindow->factory().getGameObjects()) {
            if (QString::fromStdString(entity->mName) == data) {
                parentID = entity->eID;
                break;
            }
        }
        //    // Hooo boy... This sets the parent ID in the dragged entity to be the ID of the entity it was dropped onto.
        TransformComponent *comp = dynamic_cast<TransformComponent *>(mRenderWindow->factory().getComponent(Transform, selectedEntity->eID));
        if (comp) {
            comp->parentID = parentID;
            qDebug() << "New Parent Name: " + data + ". ID: " + QString::number(comp->parentID);
        }
    } else // Implies the item was dropped to the top node, aka it no longer has a parent. (or rather the parent is the top node which is empty)
        dynamic_cast<TransformComponent *>(mRenderWindow->factory().getComponent(Transform, selectedEntity->eID))->parentID = -1;
}
void MainWindow::onGameObjectClicked(const QModelIndex &index) {
    QString data = hierarchy->data(index).toString();
    onGameObjectDragged(data);

    // Implement properties(components) list update here
}
void MainWindow::onGameObjectDragged(const QString &text) {
    for (auto entity : mRenderWindow->factory().getGameObjects()) {
        if (QString::fromStdString(entity->mName) == text) {
            selectedEntity = entity;
            break;
        }
    }
    qDebug() << "Name: " + QString::fromStdString(selectedEntity->mName) + ". ID: " + QString::number(selectedEntity->eID);
}
void MainWindow::onNameChanged(const QModelIndex &index) {
    if (selectedEntity)
        selectedEntity->mName = hierarchy->data(index).toString().toStdString();
}
void MainWindow::onGameObjectsChanged(GLuint entity) {
    QStandardItem *parentItem = hierarchy->invisibleRootItem();
    GameObject *object = mRenderWindow->factory().getGameObject(entity);
    QStandardItem *item = new QStandardItem(QString(QString::fromStdString(object->mName)));
    parentItem->appendRow(item);
}
/**
 * @brief Initial insertion of gameobjects, such as those made in an init function or read from a level file.
 * @param entities
 */
void MainWindow::insertGameObjects(std::vector<int> entities) {
    QStandardItem *parentItem = hierarchy->invisibleRootItem();
    for (auto entity : entities) {
        if (entity != -1) {
            GameObject *object = mRenderWindow->factory().getGameObject(entity);
            QStandardItem *item;
            if (object->mName == "")
                item = new QStandardItem(QString("GameObject")) /*.arg(idx)*/;
            else
                item = new QStandardItem(QString(QString::fromStdString(object->mName))) /*.arg(idx)*/;
            int parentID = dynamic_cast<TransformComponent *>(mRenderWindow->factory().getComponent(Transform, object->eID))->parentID;
            if (parentID != -1) {
                QString parent = QString::fromStdString(mRenderWindow->factory().getGameObject(entities.at(parentID))->mName);
                forEach(hierarchy, parent, item);
            } else
                parentItem->appendRow(item);
        }
    }
}
/**
 * @brief Iterate through a model to find a specific item
 * @param model
 * @param parentName
 * @param child
 * @param parent
 */
void MainWindow::forEach(QAbstractItemModel *model, QString parentName, QStandardItem *child, QModelIndex parent) {
    for (int r = 0; r < model->rowCount(parent); ++r) {
        QModelIndex index = model->index(r, 0, parent);
        QVariant name = model->data(index);
        if (parentName == name) { // If in-value parentName matches the name of the current item, append the new child to that item. In theory.
            QStandardItem *parentItem = hierarchy->itemFromIndex(index);
            parentItem->appendRow(child);
            break;
        }
        if (model->hasChildren(index)) {
            forEach(model, parentName, child, index);
        }
    }
}
