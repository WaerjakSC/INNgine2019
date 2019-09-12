#include "mainwindow.h"
#include "GUI/hierarchymodel.h"
#include "innpch.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QSurfaceFormat>

#include "renderwindow.h"

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

    connect(ui->SceneHierarchy, &QListView::clicked, this, &MainWindow::onGameObjectClicked);
    //    connect(ui->SceneHierarchy, &QListView::dropEvent, this, &MainWindow::onGameObjectClicked);

    connect(hierarchy, &QStringListModel::dataChanged, this, &MainWindow::onNameChanged);
}

//Example of a slot called from the button on the top of the program.
void MainWindow::on_pushButton_clicked() {
    mRenderWindow->toggleWireframe();
}

void MainWindow::onGameObjectClicked(const QModelIndex &index) {
    QString data = hierarchy->data(index).toString();
    for (auto entity : mRenderWindow->factory().getGameObjects()) {
        if (QString::fromStdString(entity->mName) == data) {
            selectedEntity = entity;
            break;
        }
    }
    qDebug() << "Name: " + QString::fromStdString(selectedEntity->mName) + ". ID: " + QString::number(selectedEntity->eID);

    // Implement properties(components) list update here
}
void MainWindow::onNameChanged(const QModelIndex &index) {
    selectedEntity->mName = hierarchy->data(index).toString().toStdString();
}
void MainWindow::onGameObjectsChanged() {
    //    ui->SceneHierarchy
}
void MainWindow::insertGameObjects(std::vector<GameObject *> entities) {
    QStandardItem *parentItem = hierarchy->invisibleRootItem();
    int idx = 0;
    for (auto entity : entities) {
        QStandardItem *item = new QStandardItem(QString(QString::fromStdString(entity->mName)));
        parentItem->appendRow(item);
        parentItem = item;
        idx++;
        //        if(entity->hasParent)
        //                {
        //                    // With the entityID of parent, find the row of that item and use parent->appendRow(item);
        //                }
    }
}
