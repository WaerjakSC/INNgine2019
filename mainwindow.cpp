#include "mainwindow.h"
#include "Systems/rendersystem.h"
#include "hierarchymodel.h"
#include "innpch.h"
#include "movementsystem.h"
#include "registry.h"
#include "renderwindow.h"
#include "ui_mainwindow.h"
#include "verticalscrollarea.h"
#include <QComboBox>
#include <QDesktopWidget>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QStandardItem>
#include <QStyleFactory>
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
    hView->setMainWindow(this);
    scrollArea = new VerticalScrollArea();
    ui->horizontalTopLayout->addWidget(scrollArea);

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
    connect(mRenderWindow, &RenderWindow::snapSignal, this, &MainWindow::snapToObject);
}
void MainWindow::updatePositionVals(gsl::Vector3D newPos) {
    emit posX(newPos.x);
    emit posY(newPos.y);
    emit posZ(newPos.z);
}
void MainWindow::updateRotationVals(gsl::Vector3D newRot) {
    emit rotX(newRot.x);
    emit rotY(newRot.y);
    emit rotZ(newRot.z);
}
void MainWindow::updateScaleVals(gsl::Vector3D newScale) {
    emit scaleX(newScale.x);
    emit scaleY(newScale.y);
    emit scaleZ(newScale.z);
}
void MainWindow::setNewShader(const QString &text) {
    if (text == "Texture")
        emit newShader(selectedEntity->eID, Tex);
    else if (text == "Color")
        emit newShader(selectedEntity->eID, Color);
    else if (text == "Phong")
        emit newShader(selectedEntity->eID, Phong);
}
void MainWindow::snapToObject() {
    if (selectedEntity)
        mRenderWindow->snapToObject(selectedEntity->eID);
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
    // Component menu actions don't do anything *yet* //
    QMenu *components = ui->menuBar->addMenu(tr("&Components")); // Maybe disable specific component if selected entity has that component already
    QAction *transform = new QAction(tr("Transform"), this);
    components->addAction(transform);
    QAction *material = new QAction(tr("Material"), this);
    components->addAction(material);
    QAction *mesh = new QAction(tr("Mesh"), this); // Somewhere along this action path should let you choose the mesh file to use/import
    components->addAction(mesh);
    QAction *light = new QAction(tr("Light"), this);
    components->addAction(light);
    QAction *input = new QAction(tr("Input"), this);
    components->addAction(input);
    QAction *physics = new QAction(tr("Physics"), this);
    components->addAction(physics);
    QAction *sound = new QAction(tr("Sound"), this);
    components->addAction(sound);

    ui->mainToolBar->setMovable(false);

    connect(cube, &QAction::triggered, this, &MainWindow::makeCube);
    connect(sphere, &QAction::triggered, this, &MainWindow::makeSphere);
    connect(plane, &QAction::triggered, this, &MainWindow::makePlane);
    connect(this, &MainWindow::made3DObject, this, &MainWindow::onGameObjectsChanged);
}
/**
 * @brief When a gameobject is selected, show all its components in separate groupboxes in the rightmost panel.
 * @todo Dynamically update selected entity transforms etc when they change outside of the component panel
 */
void MainWindow::setupComponentList() {
    scrollArea->clearLayout();
    // Probably err, just scrap this and re-do it some other way...
    CType typeMask = ResourceManager::instance()->getGameObject(selectedEntity->eID)->types;
    if ((typeMask & CType::Transform) != CType::None) {
        setupTransformSettings(Registry::instance()->getComponent<Transform>(selectedEntity->eID));
    }
    if ((typeMask & CType::Material) != CType::None) {
        setupMaterialSettings(Registry::instance()->getComponent<Material>(selectedEntity->eID));
    }
}
void MainWindow::setupMaterialSettings(const Material &component) {
    QStyle *fusion = QStyleFactory::create("fusion");
    QGroupBox *box = new QGroupBox(tr("Material"));
    box->setAlignment(Qt::AlignCenter);
    box->setStyle(fusion);

    QVBoxLayout *shader = new QVBoxLayout;
    shader->setMargin(1);
    QComboBox *shaderType = new QComboBox;
    for (auto type : ResourceManager::instance()->getShaders()) {
        QString curText;
        switch (type.first) {
        case Color:
            curText = "Color";
            shaderType->addItem(curText);
            break;
        case Tex:
            curText = "Texture";
            shaderType->addItem("Texture");
            break;
        case Phong:
            curText = "Phong";
            shaderType->addItem("Phong");
            break;
        }
        if (type.first == component.mShader)
            shaderType->setCurrentIndex(shaderType->findText(curText));
    }
    connect(this, &MainWindow::newShader, mRenderWindow->renderer(), &RenderSystem::changeShader);
    connect(shaderType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setNewShader(const QString &)));
    // To-do: If Texture shader selected, enable a box to select the texture image.

    fileLabel = new QLabel(box);
    QString curTexture = ResourceManager::instance()->GetTextureName(Registry::instance()->getComponent<Material>(selectedEntity->eID).mTextureUnit);
    fileLabel->setText(curTexture);
    QPushButton *browseImages = new QPushButton("&Browse", this);
    browseImages->setStyle(fusion);

    connect(browseImages, &QPushButton::clicked, this, &MainWindow::setNewTextureFile);

    QHBoxLayout *texture = new QHBoxLayout;
    texture->addWidget(fileLabel);
    texture->addWidget(browseImages);
    shader->addWidget(shaderType);
    shader->addLayout(texture);
    box->setLayout(shader);
    scrollArea->addGroupBox(box);
}
void MainWindow::setNewTextureFile() {
    QString directory = QString::fromStdString(gsl::assetFilePath) + "Textures";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), directory, tr("Image Files (*.bmp)")); // Could add other image files, idk what Texture class supports
    if (!fileName.isEmpty()) {
        QFileInfo file(fileName);
        fileName = file.fileName();
        ResourceManager *factory = ResourceManager::instance();
        factory->LoadTexture(fileName.toStdString());
        Registry::instance()->getComponent<Material>(selectedEntity->eID).mTextureUnit = factory->GetTexture(fileName.toStdString())->id() - 1;
        fileLabel->setText(fileName);
    }
}
void MainWindow::setupTransformSettings(const Transform &component) {
    QStyle *fusion = QStyleFactory::create("fusion");
    QGroupBox *box = new QGroupBox(tr("Transform"));
    box->setAlignment(Qt::AlignCenter);
    box->setStyle(fusion);

    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);
    QGroupBox *posBox = new QGroupBox(tr("Position"));
    posBox->setStyle(fusion);
    posBox->setFlat(true);

    QHBoxLayout *position = new QHBoxLayout;
    position->setMargin(1);
    connect(mRenderWindow->movement(), &MovementSystem::positionChanged, this, &MainWindow::updatePositionVals);
    connect(mRenderWindow->movement(), &MovementSystem::rotationChanged, this, &MainWindow::updateRotationVals);
    connect(mRenderWindow->movement(), &MovementSystem::scaleChanged, this, &MainWindow::updateScaleVals);
    // Set up the Position Display
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            QLabel *label = new QLabel(box);
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("X:");
                break;
            case 2:
                label->setText("Y:");
                break;
            case 4:
                label->setText("Z:");
                break;
            }
            position->addWidget(label);
        } else {
            QDoubleSpinBox *val = new QDoubleSpinBox(box);
            val->setDecimals(1);
            val->setRange(-5000, 5000);
            val->setMaximumWidth(58);
            val->setStyle(fusion);
            switch (i) { // Atm shows relative position if parented to something, global if not. Should probably give the user the option to choose which to show.
            case 1:
                if (component.parentID != -1)
                    val->setValue(component.mRelativePosition.x);
                else
                    val->setValue(component.mPosition.x);
                connect(this, &MainWindow::posX, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
                break;
            case 3:
                if (component.parentID != -1)
                    val->setValue(component.mRelativePosition.y);
                else
                    val->setValue(component.mPosition.y);
                connect(this, &MainWindow::posY, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
                break;
            case 5:
                if (component.parentID != -1)
                    val->setValue(component.mRelativePosition.z);
                else
                    val->setValue(component.mPosition.z);
                connect(this, &MainWindow::posZ, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
                break;
            }
            position->addWidget(val);
        }
    }
    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    QGroupBox *rotBox = new QGroupBox(tr("Rotation"));
    rotBox->setStyle(fusion);
    rotBox->setFlat(true);

    QHBoxLayout *rotation = new QHBoxLayout;
    rotation->setMargin(1);
    // Set up the Rotation Display
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            QLabel *label = new QLabel(box);
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("X:");
                break;
            case 2:
                label->setText("Y:");
                break;
            case 4:
                label->setText("Z:");
                break;
            }
            rotation->addWidget(label);
        } else {
            QDoubleSpinBox *val = new QDoubleSpinBox(box);
            val->setDecimals(1);
            val->setRange(-180, 180);
            val->setWrapping(true);
            val->setMaximumWidth(58);
            val->setStyle(fusion);
            switch (i) {
            case 1:
                val->setValue(component.mRotation.x);
                connect(this, &MainWindow::rotX, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setRotationX(double)));
                break;
            case 3:
                val->setValue(component.mRotation.y);
                connect(this, &MainWindow::rotY, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setRotationY(double)));
                break;
            case 5:
                val->setValue(component.mRotation.z);
                connect(this, &MainWindow::rotZ, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setRotationZ(double)));
                break;
            }
            rotation->addWidget(val);
        }
    }
    rotBox->setLayout(rotation);
    grid->addWidget(rotBox, 1, 0);

    QGroupBox *scaleBox = new QGroupBox(tr("Scale"));
    scaleBox->setStyle(fusion);
    scaleBox->setFlat(true);

    QHBoxLayout *scale = new QHBoxLayout;
    scale->setMargin(1);
    // Set up the Rotation Display
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            QLabel *label = new QLabel(box);
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("X:");
                break;
            case 2:
                label->setText("Y:");
                break;
            case 4:
                label->setText("Z:");
                break;
            }
            scale->addWidget(label);
        } else { // TODO: Add lock to scale other parts proportionally
            QDoubleSpinBox *val = new QDoubleSpinBox(box);
            val->setDecimals(1);
            val->setRange(0.1, 100);
            val->setSingleStep(0.1f);
            val->setMaximumWidth(58);
            val->setStyle(fusion);
            switch (i) {
            case 1:
                val->setValue(component.mScale.x);
                connect(this, &MainWindow::scaleX, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setScaleX(double)));
                break;
            case 3:
                val->setValue(component.mScale.y);
                connect(this, &MainWindow::scaleY, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setScaleY(double)));
                break;
            case 5:
                val->setValue(component.mScale.z);
                connect(this, &MainWindow::scaleZ, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setScaleZ(double)));
                break;
            }
            scale->addWidget(val);
        }
    }
    scaleBox->setLayout(scale);
    grid->addWidget(scaleBox, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mRenderWindow->keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mRenderWindow->keyReleaseEvent(event);
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
void MainWindow::onParentChanged(const QModelIndex &parent) {
    QString data = hierarchy->data(parent).toString();
    if (data != "") {
        int parentID;
        // Find gameobject in resourcemanager and set parentID to that object's ID, then get its transformcomponent and add the childID to its list of children.
        for (auto entity : ResourceManager::instance()->getGameObjects()) {
            if (QString::fromStdString(entity->mName) == data) { // Checking by name necessitates unique names
                parentID = entity->eID;
                mRenderWindow->movement()->setParent(selectedEntity->eID, parentID);
                mRenderWindow->movement()->iterateChildren(parentID);
                qDebug() << "New Parent Name: " + QString::fromStdString(entity->mName) + ". ID: " + QString::number(parentID);
                break;
            }
        }
    } else // Implies the item was dropped to the top node, aka it no longer has a parent. (or rather the parent is the top node which is empty)
        mRenderWindow->movement()->setParent(selectedEntity->eID, -1);
}
void MainWindow::onGameObjectClicked(const QModelIndex &index) {
    QString data = hierarchy->data(index).toString();
    onGameObjectDragged(data);
    setupComponentList();

    // Implement properties(components) list update here
}
void MainWindow::onGameObjectDragged(const QString &text) {
    for (auto entity : ResourceManager::instance()->getGameObjects()) {
        if (QString::fromStdString(entity->mName) == text) {
            selectedEntity = entity;
            break;
        }
    }
    qDebug() << "Name: " + QString::fromStdString(selectedEntity->mName) + ". ID: " + QString::number(selectedEntity->eID);
    gsl::Vector3D location = mRenderWindow->movement()->getRelativePosition(selectedEntity->eID);
    qDebug() << "Location: " + QString::number(location.x) + ", " + QString::number(location.y) + ", " + QString::number(location.z);
}
void MainWindow::onNameChanged(const QModelIndex &index) {
    if (selectedEntity)
        selectedEntity->mName = hierarchy->data(index).toString().toStdString();
}
void MainWindow::onGameObjectsChanged(GLuint entity) {
    QStandardItem *parentItem = hierarchy->invisibleRootItem();
    GameObject *object = ResourceManager::instance()->getGameObject(entity);
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
            GameObject *object = ResourceManager::instance()->getGameObject(entity);
            QStandardItem *item;
            if (object->mName == "")
                item = new QStandardItem(QString("GameObject")) /*.arg(idx)*/;
            else
                item = new QStandardItem(QString(QString::fromStdString(object->mName))) /*.arg(idx)*/;
            int parentID = Registry::instance()->getComponent<Transform>(object->eID).parentID;
            if (parentID != -1) {
                QString parent = QString::fromStdString(ResourceManager::instance()->getGameObject(entities.at(parentID))->mName);
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
void MainWindow::setPositionX(double xIn) {
    mRenderWindow->movement()->setPositionX(selectedEntity->eID, xIn);
}
void MainWindow::setPositionY(double yIn) {
    mRenderWindow->movement()->setPositionY(selectedEntity->eID, yIn);
}
void MainWindow::setPositionZ(double zIn) {
    mRenderWindow->movement()->setPositionZ(selectedEntity->eID, zIn);
}
void MainWindow::setRotationX(double xIn) {
    mRenderWindow->movement()->setRotationX(selectedEntity->eID, xIn);
}
void MainWindow::setRotationY(double yIn) {
    mRenderWindow->movement()->setRotationY(selectedEntity->eID, yIn);
}
void MainWindow::setRotationZ(double zIn) {
    mRenderWindow->movement()->setRotationZ(selectedEntity->eID, zIn);
}
void MainWindow::setScaleX(double xIn) {
    mRenderWindow->movement()->setScaleX(selectedEntity->eID, xIn);
}
void MainWindow::setScaleY(double yIn) {
    mRenderWindow->movement()->setScaleY(selectedEntity->eID, yIn);
}
void MainWindow::setScaleZ(double zIn) {
    mRenderWindow->movement()->setScaleZ(selectedEntity->eID, zIn);
}
