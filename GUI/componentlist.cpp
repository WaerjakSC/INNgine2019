#include "componentlist.h"
#include "components.h"
#include "constants.h"
#include "hierarchymodel.h"
#include "hierarchyview.h"
#include "mainwindow.h"
#include "movementsystem.h"
#include "registry.h"
#include "rendersystem.h"
#include "renderwindow.h"
#include "verticalscrollarea.h"
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QStyleFactory>
ComponentList::ComponentList(MainWindow *window, VerticalScrollArea *inScrollArea)
    : registry(Registry::instance()), scrollArea(inScrollArea), mMainWindow(window) {
}
/**
 * @brief When a gameobject is selected, show all its components in separate groupboxes in the rightmost panel.
 * @todo Right click a QGroupBox to get options like remove component (look at Unity for inspiration)
 */
void ComponentList::setupComponentList() {
    scrollArea->clearLayout();
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Transform) != CType::None) {
        setupTransformSettings(registry->getComponent<Transform>(mMainWindow->selectedEntity->id()));
    }
    if ((typeMask & CType::Material) != CType::None) {
        setupMaterialSettings(registry->getComponent<Material>(mMainWindow->selectedEntity->id()));
    }
    if ((typeMask & CType::Mesh) != CType::None) {
        setupMeshSettings(registry->getComponent<Mesh>(mMainWindow->selectedEntity->id()));
    }
}
void ComponentList::addTransformComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Transform) == CType::None)
        registry->addComponent<Transform>(mMainWindow->selectedEntity->id());
    setupComponentList();
}
void ComponentList::addMaterialComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Material) == CType::None)
        registry->addComponent<Material>(mMainWindow->selectedEntity->id());
    setupComponentList();
}
void ComponentList::addMeshComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Mesh) == CType::None)
        registry->addComponent<Mesh>(mMainWindow->selectedEntity->id());
    setupComponentList();
}
void ComponentList::addLightComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Light) == CType::None)
        registry->addComponent<Light>(mMainWindow->selectedEntity->id());
    setupComponentList();
}
void ComponentList::addInputComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Input) == CType::None)
        registry->addComponent<Input>(mMainWindow->selectedEntity->id());
    setupComponentList();
}
void ComponentList::addPhysicsComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Physics) == CType::None)
        registry->addComponent<Physics>(mMainWindow->selectedEntity->id());
    setupComponentList();
}
void ComponentList::addSoundComponent() {
    CType typeMask = mMainWindow->selectedEntity->types();
    if ((typeMask & CType::Sound) == CType::None)
        registry->addComponent<Sound>(mMainWindow->selectedEntity->id());
    setupComponentList();
}

void ComponentList::setupMeshSettings(const Mesh &mesh) {
    QStyle *fusion = QStyleFactory::create("fusion");
    QGroupBox *box = new QGroupBox(tr("Mesh"));
    box->setAlignment(Qt::AlignCenter);
    box->setStyle(fusion);

    objFileLabel = new QLabel(box);
    objFileLabel->setText(ResourceManager::instance()->getMeshName(mesh));
    QPushButton *button = new QPushButton("Change Mesh");
    connect(button, &QPushButton::clicked, this, &ComponentList::setNewMesh);

    QHBoxLayout *meshLayout = new QHBoxLayout;
    meshLayout->setMargin(1);
    meshLayout->addWidget(objFileLabel);
    meshLayout->addWidget(button);
    box->setLayout(meshLayout);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupMaterialSettings(const Material &component) {
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
            break;
        case Tex:
            curText = "Texture";
            break;
        case Phong:
            curText = "Phong";
            break;
        default:
            curText = "Phong";
            break;
        }
        shaderType->addItem(curText);
        if (type.first == component.mShader)
            shaderType->setCurrentIndex(shaderType->findText(curText));
    }
    connect(this, &ComponentList::newShader, registry->getSystem<RenderSystem>().get(), &RenderSystem::changeShader);
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
    connect(browseImages, &QPushButton::clicked, this, &ComponentList::setNewTextureFile);
    QHBoxLayout *texture = new QHBoxLayout;
    texture->addWidget(textureThumb);
    texture->addWidget(texFileLabel);
    texture->addWidget(browseImages);

    colorLabel = new QLabel;
    QPixmap curColor(18, 18);
    vec3 oColor = component.mObjectColor;
    rgb.setRgbF(oColor.x, oColor.y, oColor.z); // setRgbF takes floats in the 0-1 range, which is what we want
    curColor.fill(rgb);
    colorLabel->setPixmap(curColor);
    QPushButton *colorButton = new QPushButton(tr("Change Color"));
    connect(colorButton, &QPushButton::clicked, this, &ComponentList::setColor);

    QHBoxLayout *color = new QHBoxLayout;
    color->addWidget(colorLabel);
    color->addWidget(colorButton);

    shader->addWidget(shaderType);
    shader->addLayout(texture);
    shader->addLayout(color);
    box->setLayout(shader);
    scrollArea->addGroupBox(box);
}
void ComponentList::setNewShader(const QString &text) {
    if (text == "Texture")
        emit newShader(mMainWindow->selectedEntity->id(), Tex);
    else if (text == "Color")
        emit newShader(mMainWindow->selectedEntity->id(), Color);
    else if (text == "Phong")
        emit newShader(mMainWindow->selectedEntity->id(), Phong);
}
void ComponentList::setNewTextureFile() {
    QString directory = QString::fromStdString(gsl::assetFilePath) + "Textures";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), directory, tr("Image Files (*.bmp)")); // Could add other image files, idk what Texture class supports
    if (!fileName.isEmpty()) {
        QFileInfo file(fileName);
        fileName = file.fileName();
        ResourceManager *factory = ResourceManager::instance();
        factory->loadTexture(fileName.toStdString());
        registry->getComponent<Material>(mMainWindow->selectedEntity->id()).mTextureUnit = factory->getTexture(fileName.toStdString())->id() - 1;
        texFileLabel->setText(fileName);
    }
}
void ComponentList::setNewMesh() {
    QString directory = QString::fromStdString(gsl::assetFilePath) + "Meshes";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Mesh File"), directory, tr("OBJ Files (*.obj)"));
    if (!fileName.isEmpty()) {
        QFileInfo file(fileName);
        fileName = file.fileName();
        ResourceManager *factory = ResourceManager::instance();
        factory->setMesh(fileName.toStdString(), mMainWindow->selectedEntity->id());
        objFileLabel->setText(fileName);
    }
}
/**
 * @brief Not sure why this gives a geometry warning, ask Ole?
 */
void ComponentList::setColor() {
    const QColor color = QColorDialog::getColor(rgb, this, "Select Color");
    if (color.isValid()) {
        QPixmap newRgb(18, 18);
        newRgb.fill(color);
        colorLabel->setPixmap(newRgb);
    }
    registry->getComponent<Material>(mMainWindow->selectedEntity->id()).mObjectColor = vec3(color.redF(), color.greenF(), color.blueF());
}
void ComponentList::setupTransformSettings(const Transform &component) {
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
    MovementSystem *movement = registry->getSystem<MovementSystem>().get();
    connect(movement, &MovementSystem::positionChanged, this, &ComponentList::updatePositionVals);
    connect(movement, &MovementSystem::rotationChanged, this, &ComponentList::updateRotationVals);
    connect(movement, &MovementSystem::scaleChanged, this, &ComponentList::updateScaleVals);
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

            switch (i) { // Atm shows relative position if parented to something, global if not. Should probably give the user the option to choose which to show.
            case 1:
                xVal = new QDoubleSpinBox(box);
                xVal->setDecimals(1);
                xVal->setRange(-5000, 5000);
                xVal->setMaximumWidth(58);
                xVal->setStyle(fusion);
                xVal->setValue(component.localPosition.x);
                connect(this, &ComponentList::posX, xVal, &QDoubleSpinBox::setValue);
                connect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
                position->addWidget(xVal);
                break;
            case 3:
                yVal = new QDoubleSpinBox(box);
                yVal->setDecimals(1);
                yVal->setRange(-5000, 5000);
                yVal->setMaximumWidth(58);
                yVal->setStyle(fusion);
                yVal->setValue(component.localPosition.y);
                connect(this, &ComponentList::posY, yVal, &QDoubleSpinBox::setValue);
                connect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
                position->addWidget(yVal);
                break;
            case 5:
                zVal = new QDoubleSpinBox(box);
                zVal->setDecimals(1);
                zVal->setRange(-5000, 5000);
                zVal->setMaximumWidth(58);
                zVal->setStyle(fusion);
                zVal->setValue(component.localPosition.z);
                connect(this, &ComponentList::posZ, zVal, &QDoubleSpinBox::setValue);
                connect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
                position->addWidget(zVal);
                break;
            }
        }
    }
    posBox->setLayout(position);
    QHBoxLayout *check = new QHBoxLayout;
    abs = new QCheckBox(tr("Absolute Position"));
    connect(abs, &QCheckBox::stateChanged, this, &ComponentList::updatePosSpinBoxes);
    check->addWidget(abs);
    grid->addWidget(posBox, 0, 0);
    grid->addLayout(check, 1, 0);

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
                val->setValue(component.localRotation.x);
                connect(this, &ComponentList::rotX, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setRotationX(double)));
                break;
            case 3:
                val->setValue(component.localRotation.y);
                connect(this, &ComponentList::rotY, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setRotationY(double)));
                break;
            case 5:
                val->setValue(component.localRotation.z);
                connect(this, &ComponentList::rotZ, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setRotationZ(double)));
                break;
            }
            rotation->addWidget(val);
        }
    }
    rotBox->setLayout(rotation);
    grid->addWidget(rotBox, 2, 0);

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
                val->setValue(component.localScale.x);
                connect(this, &ComponentList::scaleX, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setScaleX(double)));
                break;
            case 3:
                val->setValue(component.localScale.y);
                connect(this, &ComponentList::scaleY, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setScaleY(double)));
                break;
            case 5:
                val->setValue(component.localScale.z);
                connect(this, &ComponentList::scaleZ, val, &QDoubleSpinBox::setValue);
                connect(val, SIGNAL(valueChanged(double)), this, SLOT(setScaleZ(double)));
                break;
            }
            scale->addWidget(val);
        }
    }
    scaleBox->setLayout(scale);
    grid->addWidget(scaleBox, 3, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::updatePosSpinBoxes(int state) {
    disconnect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
    disconnect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
    disconnect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
    auto &trans = registry->getComponent<Transform>(mMainWindow->selectedEntity->id());
    switch (state) {
    case 0:
        xVal->setValue(trans.localPosition.x);
        yVal->setValue(trans.localPosition.y);
        zVal->setValue(trans.localPosition.z);
        break;
    case 2:
        registry->getSystem<MovementSystem>()->getAbsolutePosition(mMainWindow->selectedEntity->id()); // have to call this function once to update the global pos variable if it hasn't been cached yet
        xVal->setValue(trans.position.x);
        yVal->setValue(trans.position.y);
        zVal->setValue(trans.position.z);
        break;
    default:
        break;
    }
    connect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
    connect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
    connect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
}
void ComponentList::updatePositionVals(GLuint eID, vec3 newPos, bool isGlobal) {
    if (abs->isChecked() != isGlobal) // if absolute position is checked but the signal is local, don't do anything, and vice versa
        return;
    if (eID == mMainWindow->selectedEntity->id()) {
        emit posX(newPos.x);
        emit posY(newPos.y);
        emit posZ(newPos.z);
    }
}
void ComponentList::updateRotationVals(GLuint eID, vec3 newRot) {
    if (eID == mMainWindow->selectedEntity->id()) {
        emit rotX(newRot.x);
        emit rotY(newRot.y);
        emit rotZ(newRot.z);
    }
}
void ComponentList::updateScaleVals(GLuint eID, vec3 newScale) {
    if (eID == mMainWindow->selectedEntity->id()) {
        emit scaleX(newScale.x);
        emit scaleY(newScale.y);
        emit scaleZ(newScale.z);
    }
}
void ComponentList::setPositionX(double xIn) {
    auto movement = registry->getSystem<MovementSystem>();
    if (abs->isChecked())
        movement->setAbsolutePositionX(mMainWindow->selectedEntity->id(), xIn, false);
    else
        movement->setLocalPositionX(mMainWindow->selectedEntity->id(), xIn, false);

    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setPositionY(double yIn) {
    auto movement = registry->getSystem<MovementSystem>();
    if (abs->isChecked())
        movement->setAbsolutePositionY(mMainWindow->selectedEntity->id(), yIn, false);
    else
        movement->setLocalPositionY(mMainWindow->selectedEntity->id(), yIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setPositionZ(double zIn) {
    auto movement = registry->getSystem<MovementSystem>();
    if (abs->isChecked())
        movement->setAbsolutePositionZ(mMainWindow->selectedEntity->id(), zIn, false);
    else
        movement->setLocalPositionZ(mMainWindow->selectedEntity->id(), zIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setRotationX(double xIn) {
    auto movement = registry->getSystem<MovementSystem>();
    movement->setRotationX(mMainWindow->selectedEntity->id(), xIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setRotationY(double yIn) {
    auto movement = registry->getSystem<MovementSystem>();
    movement->setRotationY(mMainWindow->selectedEntity->id(), yIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setRotationZ(double zIn) {
    auto movement = registry->getSystem<MovementSystem>();
    movement->setRotationZ(mMainWindow->selectedEntity->id(), zIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setScaleX(double xIn) {
    auto movement = registry->getSystem<MovementSystem>();
    movement->setScaleX(mMainWindow->selectedEntity->id(), xIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setScaleY(double yIn) {
    auto movement = registry->getSystem<MovementSystem>();
    movement->setScaleY(mMainWindow->selectedEntity->id(), yIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
void ComponentList::setScaleZ(double zIn) {
    auto movement = registry->getSystem<MovementSystem>();
    movement->setScaleZ(mMainWindow->selectedEntity->id(), zIn, false);
    if (!mMainWindow->mRenderWindow->isPlaying())
        movement->updateEntity(mMainWindow->selectedEntity->id());
}
