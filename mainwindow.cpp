#include "mainwindow.h"
#include "Systems/rendersystem.h"
#include "entityitem.h"
#include "hierarchymodel.h"
#include "innpch.h"
#include "movementsystem.h"
#include "registry.h"
#include "renderwindow.h"
#include "ui_mainwindow.h"
#include "verticalscrollarea.h"
#include <QColorDialog>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
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

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    mRenderWindowContainer->setFocus();

    //Set size of program in % of available screen
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    connect(hierarchy, &HierarchyModel::dataChanged, this, &MainWindow::onNameChanged);
    connect(hierarchy, &HierarchyModel::parentChanged, this, &MainWindow::onParentChanged);
    connect(hView, &HierarchyView::dragSelection, this, &MainWindow::onGameObjectDragged);
    connect(hView, &HierarchyView::clicked, this, &MainWindow::onGameObjectClicked);
    connect(mRenderWindow, &RenderWindow::snapSignal, this, &MainWindow::snapToObject);
}
void MainWindow::updatePositionVals(GLuint eID, gsl::Vector3D newPos) {
    if (eID == selectedEntity->eID) {
        emit posX(newPos.x);
        emit posY(newPos.y);
        emit posZ(newPos.z);
    }
}
void MainWindow::updateRotationVals(GLuint eID, gsl::Vector3D newRot) {
    if (eID == selectedEntity->eID) {
        emit rotX(newRot.x);
        emit rotY(newRot.y);
        emit rotZ(newRot.z);
    }
}
void MainWindow::updateScaleVals(GLuint eID, gsl::Vector3D newScale) {
    if (eID == selectedEntity->eID) {
        emit scaleX(newScale.x);
        emit scaleY(newScale.y);
        emit scaleZ(newScale.z);
    }
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
// To-do: Save As button, folder pop-up that lets you choose or make a new scene file.
void MainWindow::createActions() {
    QMenu *projectActions = ui->menuBar->addMenu(tr("Project"));
    QAction *saveScene = new QAction(tr("Save Scene"));
    QAction *loadScene = new QAction(tr("Load Scene"));
    projectActions->addAction(saveScene);
    projectActions->addAction(loadScene);
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
    // Component menu actions don't do anything *yet* //
    QMenu *components = ui->menuBar->addMenu(tr("Add Components")); // Maybe disable specific component if selected entity has that component already
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

    connect(saveScene, &QAction::triggered, mRenderWindow, &RenderWindow::save);
    connect(loadScene, &QAction::triggered, mRenderWindow, &RenderWindow::load);

    // Setup the component actions
    connect(transform, &QAction::triggered, this, &MainWindow::addTransformComponent);
    connect(material, &QAction::triggered, this, &MainWindow::addMaterialComponent);
    connect(mesh, &QAction::triggered, this, &MainWindow::addMeshComponent);
    connect(light, &QAction::triggered, this, &MainWindow::addLightComponent);
    connect(input, &QAction::triggered, this, &MainWindow::addInputComponent);
    connect(physics, &QAction::triggered, this, &MainWindow::addPhysicsComponent);
    connect(sound, &QAction::triggered, this, &MainWindow::addSoundComponent);

    connect(cube, &QAction::triggered, this, &MainWindow::makeCube);
    connect(sphere, &QAction::triggered, this, &MainWindow::makeSphere);
    connect(plane, &QAction::triggered, this, &MainWindow::makePlane);
    connect(empty, &QAction::triggered, this, &MainWindow::makeGameObject);

    connect(this, &MainWindow::made3DObject, this, &MainWindow::onGameObjectsChanged);
}
void MainWindow::addTransformComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Transform) == CType::None)
        registry->addComponent<Transform>(selectedEntity->eID);
    setupComponentList();
}
void MainWindow::addMaterialComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Material) == CType::None)
        registry->addComponent<Material>(selectedEntity->eID);
    setupComponentList();
}
void MainWindow::addMeshComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Mesh) == CType::None)
        registry->addComponent<Mesh>(selectedEntity->eID);
    setupComponentList();
}
void MainWindow::addLightComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Light) == CType::None)
        registry->addComponent<Light>(selectedEntity->eID);
    setupComponentList();
}
void MainWindow::addInputComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Input) == CType::None)
        registry->addComponent<Input>(selectedEntity->eID);
    setupComponentList();
}
void MainWindow::addPhysicsComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Physics) == CType::None)
        registry->addComponent<Physics>(selectedEntity->eID);
    setupComponentList();
}
void MainWindow::addSoundComponent() {
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Sound) == CType::None)
        registry->addComponent<Sound>(selectedEntity->eID);
    setupComponentList();
}
/**
 * @brief When a gameobject is selected, show all its components in separate groupboxes in the rightmost panel.
 * @todo Right click a QGroupBox to get options like remove component (look at Unity for inspiration)
 */
void MainWindow::setupComponentList() {
    scrollArea->clearLayout();
    Registry *registry = Registry::instance();
    CType typeMask = selectedEntity->types;
    if ((typeMask & CType::Transform) != CType::None) {
        setupTransformSettings(registry->getComponent<Transform>(selectedEntity->eID));
    }
    if ((typeMask & CType::Material) != CType::None) {
        setupMaterialSettings(registry->getComponent<Material>(selectedEntity->eID));
    }
    if ((typeMask & CType::Mesh) != CType::None) {
        setupMeshSettings(registry->getComponent<Mesh>(selectedEntity->eID));
    }
}
void MainWindow::setupMeshSettings(const Mesh &mesh) {
    QStyle *fusion = QStyleFactory::create("fusion");
    QGroupBox *box = new QGroupBox(tr("Mesh"));
    box->setAlignment(Qt::AlignCenter);
    box->setStyle(fusion);

    objFileLabel = new QLabel(box);
    objFileLabel->setText(ResourceManager::instance()->getMeshName(mesh));
    QPushButton *button = new QPushButton("Change Mesh");
    connect(button, &QPushButton::clicked, this, &MainWindow::setNewMesh);

    QHBoxLayout *meshLayout = new QHBoxLayout;
    meshLayout->setMargin(1);
    meshLayout->addWidget(objFileLabel);
    meshLayout->addWidget(button);
    box->setLayout(meshLayout);
    scrollArea->addGroupBox(box);
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

    QString curTexture = ResourceManager::instance()->getTextureName(component.mTextureUnit);
    QLabel *textureThumb = new QLabel(box);
    QPixmap thumbNail;
    thumbNail.load(QString::fromStdString(gsl::assetFilePath) + "Textures/" + curTexture); // Load the texture image into the pixmap
    textureThumb->setPixmap(thumbNail.scaled(18, 18));                                     // Get a scaled version of the image loaded above
    texFileLabel = new QLabel(box);
    texFileLabel->setText(curTexture); // Saves the file name of the texture image
    QPushButton *browseImages = new QPushButton("Browse", this);
    browseImages->setStyle(fusion);
    connect(browseImages, &QPushButton::clicked, this, &MainWindow::setNewTextureFile);
    QHBoxLayout *texture = new QHBoxLayout;
    texture->addWidget(textureThumb);
    texture->addWidget(texFileLabel);
    texture->addWidget(browseImages);

    colorLabel = new QLabel;
    QPixmap curColor(18, 18);
    gsl::Vector3D oColor = component.mObjectColor;
    rgb.setRgbF(oColor.x, oColor.y, oColor.z); // setRgbF takes floats in the 0-1 range, which is what we want
    curColor.fill(rgb);
    colorLabel->setPixmap(curColor);
    QPushButton *colorButton = new QPushButton(tr("Change Color"));
    connect(colorButton, &QPushButton::clicked, this, &MainWindow::setColor);

    QHBoxLayout *color = new QHBoxLayout;
    color->addWidget(colorLabel);
    color->addWidget(colorButton);

    shader->addWidget(shaderType);
    shader->addLayout(texture);
    shader->addLayout(color);
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
        factory->loadTexture(fileName.toStdString());
        Registry::instance()->getComponent<Material>(selectedEntity->eID).mTextureUnit = factory->getTexture(fileName.toStdString())->id() - 1;
        texFileLabel->setText(fileName);
    }
}
void MainWindow::setNewMesh() {
    QString directory = QString::fromStdString(gsl::assetFilePath) + "Meshes";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Mesh File"), directory, tr("OBJ Files (*.obj)"));
    if (!fileName.isEmpty()) {
        QFileInfo file(fileName);
        fileName = file.fileName();
        ResourceManager *factory = ResourceManager::instance();
        factory->setMesh(fileName.toStdString(), selectedEntity->eID);
        objFileLabel->setText(fileName);
    }
}
/**
 * @brief Not sure why this gives a geometry warning, ask Ole?
 */
void MainWindow::setColor() {
    const QColor color = QColorDialog::getColor(rgb, this, "Select Color");
    if (color.isValid()) {
        QPixmap newRgb(18, 18);
        newRgb.fill(color);
        colorLabel->setPixmap(newRgb);
    }
    Registry::instance()->getComponent<Material>(selectedEntity->eID).mObjectColor = gsl::Vector3D(color.redF(), color.greenF(), color.blueF());
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
void MainWindow::makeGameObject() {
    emit made3DObject(ResourceManager::instance()->makeGameObject("GameObject"));
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
    GLuint id = static_cast<EntityItem *>(hierarchy->itemFromIndex(index))->id();
    onGameObjectDragged(id);
    setupComponentList();
}
/**
 * @brief Get the game object the user is interacting with.
 * @param id of the entity
 */
void MainWindow::onGameObjectDragged(GLuint id) {
    selectedEntity = ResourceManager::instance()->getGameObject(id);
    qDebug() << "Name: " + QString::fromStdString(selectedEntity->mName) + ". ID: " + QString::number(selectedEntity->eID);
    if ((selectedEntity->types & CType::Transform) != CType::None) {
        gsl::Vector3D location;
        if (mRenderWindow->movement()->hasParent(selectedEntity->eID)) {
            location = mRenderWindow->movement()->getRelativePosition(selectedEntity->eID);
        } else
            location = mRenderWindow->movement()->getPosition(selectedEntity->eID);
        qDebug() << "Location: " + QString::number(location.x) + ", " + QString::number(location.y) + ", " + QString::number(location.z);
    }
}
void MainWindow::onNameChanged(const QModelIndex &index) {
    if (selectedEntity)
        selectedEntity->mName = hierarchy->data(index).toString().toStdString();
}
void MainWindow::onGameObjectsChanged(GLuint entity) {
    EntityItem *parentItem = static_cast<EntityItem *>(hierarchy->invisibleRootItem());
    GameObject *object = ResourceManager::instance()->getGameObject(entity);
    EntityItem *item = new EntityItem(QString::fromStdString(object->mName), object->eID);
    parentItem->appendRow(item);
}
/**
 * @brief Initial insertion of gameobjects, such as those made in an init function or read from a level file.
 * @param entities
 */
void MainWindow::insertGameObjects() {
    EntityItem *parentItem = static_cast<EntityItem *>(hierarchy->invisibleRootItem());
    for (auto entity : ResourceManager::instance()->getGameObjects()) {
        EntityItem *item;
        if (entity->mName == "")
            item = new EntityItem(QString("GameObject"), entity->eID) /*.arg(idx)*/;
        else
            item = new EntityItem(QString::fromStdString(entity->mName), entity->eID) /*.arg(idx)*/;
        int parentID = Registry::instance()->getComponent<Transform>(entity->eID).parentID;
        if (parentID != -1) {
            QString parent = QString::fromStdString(ResourceManager::instance()->getGameObject(parentID)->mName);
            forEach(hierarchy, parent, item);
        } else
            parentItem->appendRow(item);
    }
}
void MainWindow::removeGameObject(const QModelIndex &index) {
}
/**
 * @brief Iterate through a model to find a specific item
 * @param model
 * @param parentName
 * @param child
 * @param parent
 */
void MainWindow::forEach(QAbstractItemModel *model, QString parentName, EntityItem *child, QModelIndex parent) {
    for (int r = 0; r < model->rowCount(parent); ++r) {
        QModelIndex index = model->index(r, 0, parent);
        QVariant name = model->data(index);
        if (parentName == name) { // If in-value parentName matches the name of the current item, append the new child to that item. In theory.
            EntityItem *parentItem = static_cast<EntityItem *>(hierarchy->itemFromIndex(index));
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
