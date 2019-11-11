#include "componentlist.h"
#include "collisionsystem.h"
#include "componentgroupbox.h"
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
    fusion = QStyleFactory::create("fusion");
}
/**
 * @brief When an entity is selected, show all its components in separate groupboxes in the rightmost panel.
 * Order of if-statements is the order the component list will show each component
 */
void ComponentList::setupComponentList() {
    scrollArea->clearLayout();
    GLuint eID = mMainWindow->selectedEntity->id();
    if (registry->contains<Transform>(eID)) {
        setupTransformSettings(registry->get<Transform>(eID));
    }
    if (registry->contains<AIcomponent>(eID)) {
        setupAISettings(registry->get<AIcomponent>(eID));
    }
    if (registry->contains<AABB>(eID)) {
        setupAABBSettings(registry->get<AABB>(eID));
    }
    if (registry->contains<OBB>(eID)) {
        setupOBBSettings(registry->get<OBB>(eID));
    }
    if (registry->contains<Sphere>(eID)) {
        setupSphereColliderSettings(registry->get<Sphere>(eID));
    }
    if (registry->contains<Cylinder>(eID)) {
        setupCylinderColliderSettings(registry->get<Cylinder>(eID));
    }
    if (registry->contains<Plane>(eID)) {
        setupPlaneColliderSettings(registry->get<Plane>(eID));
    }
    if (registry->contains<Material>(eID)) {
        setupMaterialSettings(registry->get<Material>(eID));
    }
    if (registry->contains<Mesh>(eID)) {
        setupMeshSettings(registry->get<Mesh>(eID));
    }

    //    if (registry->contains<Light>(eID)) {
    //        setupTransformSettings(registry->getComponent<Light>(eID));
    //    }

    //    if (registry->contains<Transform>(eID)) {
    //        setupMeshSettings(registry->getComponent<Mesh>(eID));
    //    }
    //    if (registry->contains<Transform>(eID)) {
    //        setupTransformSettings(registry->getComponent<Transform>(eID));
    //    }
    //    if (registry->contains<Transform>(eID)) {
    //        setupMaterialSettings(registry->getComponent<Material>(eID));
    //    }
}
void ComponentList::addTransformComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Transform>(eID))
            registry->add<Transform>(eID);
        setupComponentList();
    }
}
void ComponentList::addBSplineComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<BSplinePoint>(eID))
            registry->add<BSplinePoint>(eID);
        setupComponentList();
    }
}
void ComponentList::addAIComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<AIcomponent>(eID))
            registry->add<AIcomponent>(eID);
        setupComponentList();
    }
}
void ComponentList::addMaterialComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Material>(eID))
            registry->add<Material>(eID);
        setupComponentList();
    }
}
void ComponentList::addMeshComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Mesh>(eID))
            registry->add<Mesh>(eID);
        setupComponentList();
    }
}
void ComponentList::addLightComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Light>(eID))
            registry->add<Light>(eID);
        setupComponentList();
    }
}
void ComponentList::addPhysicsComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Physics>(eID))
            registry->add<Physics>(eID);
        setupComponentList();
    }
}
void ComponentList::addSoundComponent() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Sound>(eID))
            registry->add<Sound>(eID);
        setupComponentList();
    }
}
void ComponentList::addAABBCollider() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<AABB>(eID)) {
            registry->add<AABB>(eID);
            if (registry->contains<Transform>(eID)) {
                auto &trans = registry->get<Transform>(eID);
                auto &aabb = registry->get<AABB>(eID);
                aabb.size = vec3(trans.localScale.x / 2, trans.localScale.y / 2, trans.localScale.z / 2);
            }
            registry->getSystem<MovementSystem>()->updateAABBTransform(eID);
        }
        setupComponentList();
    }
}
void ComponentList::addOBBCollider() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<OBB>(eID)) {
            registry->add<OBB>(eID);
            //            registry->getSystem<MovementSystem>()->updateColliderTransform(eID);
        }
        setupComponentList();
    }
}
void ComponentList::addSphereCollider() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Sphere>(eID)) {
            registry->add<Sphere>(eID);
            registry->getSystem<MovementSystem>()->updateSphereTransform(eID);
        }
        setupComponentList();
    }
}
void ComponentList::addPlaneCollider() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Plane>(eID)) {
            registry->add<Plane>(eID);
            //            registry->getSystem<MovementSystem>()->updateColliderTransform(eID);
        }

        setupComponentList();
    }
}
void ComponentList::addCylinderCollider() {
    if (mMainWindow->selectedEntity) {
        GLuint eID = mMainWindow->selectedEntity->id();
        if (!registry->contains<Cylinder>(eID)) {
            registry->add<Cylinder>(eID);
            //            registry->getSystem<MovementSystem>()->updateColliderTransform(eID);
        }
        setupComponentList();
    }
}
void ComponentList::setupAABBSettings(const AABB &col) {
    ComponentGroupBox *box = new ComponentGroupBox("AABB Collider", mMainWindow);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);
    QGroupBox *originBox = new QGroupBox(tr("Origin"));
    originBox->setStyle(fusion);
    originBox->setFlat(true);
    QHBoxLayout *origin = new QHBoxLayout;
    origin->setMargin(1);
    auto [originX, originY, originZ] = makeVectorBox(col.origin, origin); // Not sure yet if this is something that's supposed to be updated
    connect(this, &ComponentList::originX, originX, &QDoubleSpinBox::setValue);
    connect(originX, SIGNAL(valueChanged(double)), this, SLOT(setOriginX(double)));
    connect(this, &ComponentList::originY, originY, &QDoubleSpinBox::setValue);
    connect(originY, SIGNAL(valueChanged(double)), this, SLOT(setOriginY(double)));
    connect(this, &ComponentList::originZ, originZ, &QDoubleSpinBox::setValue);
    connect(originZ, SIGNAL(valueChanged(double)), this, SLOT(setOriginZ(double)));

    originBox->setLayout(origin);
    grid->addWidget(originBox, 0, 0);

    QGroupBox *hSizeBox = new QGroupBox(tr("Half-Size"));
    hSizeBox->setStyle(fusion);
    hSizeBox->setFlat(true);

    QHBoxLayout *hSize = new QHBoxLayout;
    hSize->setMargin(1);

    auto [sizeX, sizeY, sizeZ] = makeVectorBox(col.size, hSize, 0.1f, 5000);
    connect(this, &ComponentList::AABBSizeX, sizeX, &QDoubleSpinBox::setValue);
    connect(sizeX, SIGNAL(valueChanged(double)), this, SLOT(setAABBSizeX(double)));
    connect(this, &ComponentList::AABBSizeY, sizeY, &QDoubleSpinBox::setValue);
    connect(sizeY, SIGNAL(valueChanged(double)), this, SLOT(setAABBSizeY(double)));
    connect(this, &ComponentList::AABBSizeZ, sizeZ, &QDoubleSpinBox::setValue);
    connect(sizeZ, SIGNAL(valueChanged(double)), this, SLOT(setAABBSizeZ(double)));
    hSizeBox->setLayout(hSize);
    grid->addWidget(hSizeBox, 1, 0);

    QGroupBox *objectTypeBox = new QGroupBox(tr("Object Type"));
    makeObjectTypeBox(objectTypeBox, col);
    grid->addWidget(objectTypeBox, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupOBBSettings(const OBB &col) {
    ComponentGroupBox *box = new ComponentGroupBox("OBB Collider", mMainWindow);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);
    QGroupBox *posBox = new QGroupBox(tr("Position"));
    posBox->setStyle(fusion);
    posBox->setFlat(true);
    QHBoxLayout *position = new QHBoxLayout;
    position->setMargin(1);
    auto [positionX, positionY, positionZ] = makeVectorBox(col.position, position);
    connect(this, &ComponentList::OBBPositionX, positionX, &QDoubleSpinBox::setValue);
    connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(setOBBPositionX(double)));
    connect(this, &ComponentList::OBBPositionY, positionY, &QDoubleSpinBox::setValue);
    connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(setOBBPositionY(double)));
    connect(this, &ComponentList::OBBPositionZ, positionZ, &QDoubleSpinBox::setValue);
    connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(setOBBPositionZ(double)));
    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    QGroupBox *hSizeBox = new QGroupBox(tr("Half-Size"));
    hSizeBox->setStyle(fusion);
    hSizeBox->setFlat(true);

    QHBoxLayout *hSize = new QHBoxLayout;
    hSize->setMargin(1);

    auto [sizeX, sizeY, sizeZ] = makeVectorBox(col.size, hSize);
    connect(this, &ComponentList::OBBSizeX, sizeX, &QDoubleSpinBox::setValue);
    connect(sizeX, SIGNAL(valueChanged(double)), this, SLOT(setOBBSizeX(double)));
    connect(this, &ComponentList::OBBSizeY, sizeY, &QDoubleSpinBox::setValue);
    connect(sizeY, SIGNAL(valueChanged(double)), this, SLOT(setOBBSizeY(double)));
    connect(this, &ComponentList::OBBSizeZ, sizeZ, &QDoubleSpinBox::setValue);
    connect(sizeZ, SIGNAL(valueChanged(double)), this, SLOT(setOBBSizeZ(double)));

    // Not sure what to show for the mat3 orientation variable, maybe just convert it to a vec3 with eulers or something?

    hSizeBox->setLayout(hSize);
    grid->addWidget(hSizeBox, 1, 0);

    QGroupBox *objectTypeBox = new QGroupBox(tr("Object Type"));
    makeObjectTypeBox(objectTypeBox, col);
    grid->addWidget(objectTypeBox, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupSphereColliderSettings(const Sphere &col) {
    ComponentGroupBox *box = new ComponentGroupBox("Sphere Collider", mMainWindow);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);
    QGroupBox *posBox = new QGroupBox(tr("Position"));
    posBox->setStyle(fusion);
    posBox->setFlat(true);
    QHBoxLayout *position = new QHBoxLayout;
    position->setMargin(1);

    auto [positionX, positionY, positionZ] = makeVectorBox(col.position, position);
    connect(this, &ComponentList::spherePositionX, positionX, &QDoubleSpinBox::setValue);
    connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(setSpherePositionX(double)));
    connect(this, &ComponentList::spherePositionY, positionY, &QDoubleSpinBox::setValue);
    connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(setSpherePositionY(double)));
    connect(this, &ComponentList::spherePositionZ, positionZ, &QDoubleSpinBox::setValue);
    connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(setSpherePositionZ(double)));
    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    QGroupBox *radiusBox = new QGroupBox(tr("Radius"));
    radiusBox->setStyle(fusion);
    radiusBox->setFlat(true);

    QHBoxLayout *radiusLayout = new QHBoxLayout;
    radiusLayout->setMargin(1);
    QDoubleSpinBox *radius = new QDoubleSpinBox;
    radius->setDecimals(1);
    radius->setRange(0.1f, 5000);
    radius->setMaximumWidth(58);
    radius->setStyle(fusion);
    radius->setValue(col.radius);
    radiusLayout->addWidget(radius);
    connect(this, &ComponentList::sphereRadius, radius, &QDoubleSpinBox::setValue);
    connect(radius, SIGNAL(valueChanged(double)), this, SLOT(setSphereRadius(double)));

    radiusBox->setLayout(radiusLayout);
    grid->addWidget(radiusBox, 1, 0);

    QGroupBox *objectTypeBox = new QGroupBox(tr("Object Type"));
    makeObjectTypeBox(objectTypeBox, col);
    grid->addWidget(objectTypeBox, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void ComponentList::setupCylinderColliderSettings(const Cylinder &col) {
    ComponentGroupBox *box = new ComponentGroupBox("Cylinder Collider", mMainWindow);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);
    QGroupBox *posBox = new QGroupBox(tr("Position"));
    posBox->setStyle(fusion);
    posBox->setFlat(true);
    QHBoxLayout *position = new QHBoxLayout;
    position->setMargin(1);
    auto [positionX, positionY, positionZ] = makeVectorBox(col.position, position);
    connect(this, &ComponentList::cylinderPositionX, positionX, &QDoubleSpinBox::setValue);
    connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(setCylinderPositionX(double)));
    connect(this, &ComponentList::cylinderPositionY, positionY, &QDoubleSpinBox::setValue);
    connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(setCylinderPositionY(double)));
    connect(this, &ComponentList::cylinderPositionZ, positionZ, &QDoubleSpinBox::setValue);
    connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(setCylinderPositionZ(double)));

    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    QGroupBox *radiusBox = new QGroupBox(tr("Radius"));
    radiusBox->setStyle(fusion);
    radiusBox->setFlat(true);

    QHBoxLayout *radiusLayout = new QHBoxLayout;
    radiusLayout->setMargin(1);
    QDoubleSpinBox *radius = new QDoubleSpinBox;
    radius->setDecimals(1);
    radius->setRange(0.1f, 5000);
    radius->setMaximumWidth(58);
    radius->setStyle(fusion);
    radius->setValue(col.radius);
    radiusLayout->addWidget(radius);
    connect(this, &ComponentList::cylinderRadius, radius, &QDoubleSpinBox::setValue);
    connect(radius, SIGNAL(valueChanged(double)), this, SLOT(setCylinderRadius(double)));

    radiusBox->setLayout(radiusLayout);
    grid->addWidget(radiusBox, 1, 0);

    QGroupBox *heightBox = new QGroupBox(tr("Height"));
    heightBox->setStyle(fusion);
    heightBox->setFlat(true);

    QHBoxLayout *heightLayout = new QHBoxLayout;
    heightLayout->setMargin(1);
    QDoubleSpinBox *height = new QDoubleSpinBox;
    height->setDecimals(1);
    height->setRange(0.1f, 5000);
    height->setMaximumWidth(58);
    height->setStyle(fusion);
    height->setValue(col.height);
    heightLayout->addWidget(height);
    connect(this, &ComponentList::cylinderHeight, radius, &QDoubleSpinBox::setValue);
    connect(height, SIGNAL(valueChanged(double)), this, SLOT(setCylinderHeight(double)));

    heightBox->setLayout(heightLayout);
    grid->addWidget(heightBox, 2, 0);

    QGroupBox *objectTypeBox = new QGroupBox(tr("Object Type"));
    makeObjectTypeBox(objectTypeBox, col);
    grid->addWidget(objectTypeBox, 3, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupAISettings(const AIcomponent &ai) {
    ComponentGroupBox *box = new ComponentGroupBox("AI", mMainWindow);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);

    QGroupBox *hpBox = new QGroupBox(tr("Health"));
    hpBox->setStyle(fusion);
    hpBox->setFlat(true);

    QHBoxLayout *hpLayout = new QHBoxLayout;
    hpLayout->setMargin(1);
    QSpinBox *health = new QSpinBox;
    health->setRange(0, 5000);
    health->setMaximumWidth(58);
    health->setStyle(fusion);
    health->setValue(ai.hp);
    hpLayout->addWidget(health);
    connect(this, &ComponentList::changeHealth, health, &QSpinBox::setValue);
    connect(health, SIGNAL(valueChanged(int)), this, SLOT(setHealth(int)));

    hpBox->setLayout(hpLayout);
    grid->addWidget(hpBox, 0, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupPlaneColliderSettings(const Plane &col) {
    ComponentGroupBox *box = new ComponentGroupBox("Plane Collider", mMainWindow);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);
    QGroupBox *normalBox = new QGroupBox(tr("Normal"));
    normalBox->setStyle(fusion);
    normalBox->setFlat(true);
    QHBoxLayout *normal = new QHBoxLayout;
    normal->setMargin(1);
    auto [normalX, normalY, normalZ] = makeVectorBox(col.normal, normal);
    connect(this, &ComponentList::planeNormalX, normalX, &QDoubleSpinBox::setValue);
    connect(normalX, SIGNAL(valueChanged(double)), this, SLOT(setPlaneNormalX(double)));
    connect(this, &ComponentList::planeNormalY, normalY, &QDoubleSpinBox::setValue);
    connect(normalY, SIGNAL(valueChanged(double)), this, SLOT(setPlaneNormalY(double)));
    connect(this, &ComponentList::planeNormalZ, normalZ, &QDoubleSpinBox::setValue);
    connect(normalZ, SIGNAL(valueChanged(double)), this, SLOT(setPlaneNormalZ(double)));

    normalBox->setLayout(normal);
    grid->addWidget(normalBox, 0, 0);

    QGroupBox *distanceBox = new QGroupBox(tr("Distance"));
    distanceBox->setStyle(fusion);
    distanceBox->setFlat(true);

    QHBoxLayout *distanceLayout = new QHBoxLayout;
    distanceLayout->setMargin(1);
    QDoubleSpinBox *distance = new QDoubleSpinBox;
    distance->setDecimals(1);
    distance->setRange(0.1f, 5000);
    distance->setMaximumWidth(58);
    distance->setStyle(fusion);
    distance->setValue(col.distance);
    distanceLayout->addWidget(distance);
    connect(this, &ComponentList::planeDistance, distance, &QDoubleSpinBox::setValue);
    connect(distance, SIGNAL(valueChanged(double)), this, SLOT(setPlaneDistance(double)));

    distanceBox->setLayout(distanceLayout);
    grid->addWidget(distanceBox, 1, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupMeshSettings(const Mesh &mesh) {
    ComponentGroupBox *box = new ComponentGroupBox("Mesh", mMainWindow);

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
void ComponentList::setupMaterialSettings(const Material &mat) {
    ComponentGroupBox *box = new ComponentGroupBox("Material", mMainWindow);

    QVBoxLayout *shader = new QVBoxLayout;
    shader->setMargin(1);
    QComboBox *shaderType = new QComboBox;
    for (auto type : ResourceManager::instance()->getShaders()) {
        QString curText = QString::fromStdString(type.second->getName());
        shaderType->addItem(curText);
        if (curText == QString::fromStdString(mat.mShader->getName()))
            shaderType->setCurrentIndex(shaderType->findText(curText));
    }
    connect(this, &ComponentList::newShader, registry->getSystem<RenderSystem>().get(), &RenderSystem::changeShader);
    connect(shaderType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setNewShader(const QString &)));

    QString curTexture = ResourceManager::instance()->getTextureName(mat.mTextureUnit);
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
    vec3 oColor = mat.mObjectColor;
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
void ComponentList::setupTransformSettings(const Transform &trans) {
    ComponentGroupBox *box = new ComponentGroupBox("Transform", mMainWindow);

    QGridLayout *grid = new QGridLayout;
    grid->setMargin(2);

    MovementSystem *movement = registry->getSystem<MovementSystem>().get();
    connect(movement, &MovementSystem::positionChanged, this, &ComponentList::updatePositionVals);
    connect(movement, &MovementSystem::rotationChanged, this, &ComponentList::updateRotationVals);
    connect(movement, &MovementSystem::scaleChanged, this, &ComponentList::updateScaleVals);

    // Set up the Position display
    QGroupBox *posBox = new QGroupBox(tr("Position"));
    posBox->setStyle(fusion);
    posBox->setFlat(true);
    QHBoxLayout *position = new QHBoxLayout;
    position->setMargin(1);
    std::tie(xVal, yVal, zVal) = makeVectorBox(trans.localPosition, position);
    connect(this, &ComponentList::posX, xVal, &QDoubleSpinBox::setValue);
    connect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
    connect(this, &ComponentList::posY, yVal, &QDoubleSpinBox::setValue);
    connect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
    connect(this, &ComponentList::posZ, zVal, &QDoubleSpinBox::setValue);
    connect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    // Check box for showing absolute position instead of local position
    QHBoxLayout *check = new QHBoxLayout;
    abs = new QCheckBox(tr("Absolute Position"));
    connect(abs, &QCheckBox::stateChanged, this, &ComponentList::updatePosSpinBoxes);
    check->addWidget(abs);
    grid->addLayout(check, 1, 0);

    // Set up the Rotation Display
    QGroupBox *rotBox = new QGroupBox(tr("Rotation"));
    rotBox->setStyle(fusion);
    rotBox->setFlat(true);
    QHBoxLayout *rotation = new QHBoxLayout;
    rotation->setMargin(1);
    auto [rotX, rotY, rotZ] = makeVectorBox(trans.localRotation, rotation, -180, 180);
    connect(this, &ComponentList::rotX, rotX, &QDoubleSpinBox::setValue);
    connect(rotX, SIGNAL(valueChanged(double)), this, SLOT(setRotationX(double)));
    connect(this, &ComponentList::rotY, rotY, &QDoubleSpinBox::setValue);
    connect(rotY, SIGNAL(valueChanged(double)), this, SLOT(setRotationY(double)));
    connect(this, &ComponentList::rotZ, rotZ, &QDoubleSpinBox::setValue);
    connect(rotZ, SIGNAL(valueChanged(double)), this, SLOT(setRotationZ(double)));
    rotBox->setLayout(rotation);
    grid->addWidget(rotBox, 2, 0);

    // Set up the Rotation Display
    QGroupBox *scaleBox = new QGroupBox(tr("Scale"));
    scaleBox->setStyle(fusion);
    scaleBox->setFlat(true);
    QHBoxLayout *scale = new QHBoxLayout(box);
    scale->setMargin(1);
    auto [scaleX, scaleY, scaleZ] = makeVectorBox(trans.localScale, scale, 0.1, 100);
    connect(this, &ComponentList::scaleX, scaleX, &QDoubleSpinBox::setValue);
    connect(scaleX, SIGNAL(valueChanged(double)), this, SLOT(setScaleX(double)));
    connect(this, &ComponentList::scaleY, scaleY, &QDoubleSpinBox::setValue);
    connect(scaleY, SIGNAL(valueChanged(double)), this, SLOT(setScaleY(double)));
    connect(this, &ComponentList::scaleZ, scaleZ, &QDoubleSpinBox::setValue);
    connect(scaleZ, SIGNAL(valueChanged(double)), this, SLOT(setScaleZ(double)));
    scaleBox->setLayout(scale);
    grid->addWidget(scaleBox, 3, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void ComponentList::setNewShader(const QString &text) {
    GLuint eID = mMainWindow->selectedEntity->id();
    emit newShader(eID, text.toStdString());
}

void ComponentList::setHealth(int health) {
    auto ai = registry->get<AIcomponent>(mMainWindow->selectedEntity->id());
    ai.hp = health;
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
        registry->get<Material>(mMainWindow->selectedEntity->id()).mTextureUnit = factory->getTexture(fileName.toStdString())->id() - 1;
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
    registry->get<Material>(mMainWindow->selectedEntity->id()).mObjectColor = vec3(color.redF(), color.greenF(), color.blueF());
}
void ComponentList::updatePosSpinBoxes(int state) {
    disconnect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
    disconnect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
    disconnect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &trans = registry->get<Transform>(entityID);
    switch (state) {
    case 0:
        xVal->setValue(trans.localPosition.x);
        yVal->setValue(trans.localPosition.y);
        zVal->setValue(trans.localPosition.z);
        break;
    case 2:
        registry->getSystem<MovementSystem>()->getAbsolutePosition(entityID); // have to call this function once to update the global pos variable if it hasn't been cached yet
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
    GLuint entityID = mMainWindow->selectedEntity->id();
    if (abs->isChecked())
        movement->setAbsolutePositionX(entityID, xIn, false);
    else
        movement->setLocalPositionX(entityID, xIn, false);

    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setPositionY(double yIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    if (abs->isChecked())
        movement->setAbsolutePositionY(entityID, yIn, false);
    else
        movement->setLocalPositionY(entityID, yIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setPositionZ(double zIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    if (abs->isChecked())
        movement->setAbsolutePositionZ(entityID, zIn, false);
    else
        movement->setLocalPositionZ(entityID, zIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setRotationX(double xIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    movement->setRotationX(entityID, xIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setRotationY(double yIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    movement->setRotationY(entityID, yIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setRotationZ(double zIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    movement->setRotationZ(entityID, zIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setScaleX(double xIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    movement->setScaleX(entityID, xIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setScaleY(double yIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    movement->setScaleY(entityID, yIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setScaleZ(double zIn) {
    auto movement = registry->getSystem<MovementSystem>();
    GLuint entityID = mMainWindow->selectedEntity->id();
    movement->setScaleZ(entityID, zIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
// ************* Collider update slots *************
void ComponentList::updateAABB(GLuint eID) {
    if (!ResourceManager::instance()->isPlaying()) {
        auto movement = registry->getSystem<MovementSystem>();
        movement->updateAABBTransform(eID);
    }
}

void ComponentList::updateSphere(GLuint eID) {
    if (!ResourceManager::instance()->isPlaying()) {
        auto movement = registry->getSystem<MovementSystem>();
        movement->updateSphereTransform(eID);
    }
}
void ComponentList::setOriginX(double xIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &aabb = registry->get<AABB>(entityID);
    aabb.origin.x = xIn;
    aabb.transform.matrixOutdated = true;
    updateAABB(entityID);
}
void ComponentList::setOriginY(double yIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &aabb = registry->get<AABB>(entityID);
    aabb.origin.y = yIn;
    aabb.transform.matrixOutdated = true;
    updateAABB(entityID);
}
void ComponentList::setOriginZ(double zIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &aabb = registry->get<AABB>(entityID);
    aabb.origin.z = zIn;
    aabb.transform.matrixOutdated = true;
    updateAABB(entityID);
}
void ComponentList::setAABBSizeX(double xIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &aabb = registry->get<AABB>(entityID);
    aabb.size.x = xIn;
    aabb.transform.matrixOutdated = true;
    updateAABB(entityID);
}
void ComponentList::setAABBSizeY(double yIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &aabb = registry->get<AABB>(entityID);
    aabb.size.y = yIn;
    aabb.transform.matrixOutdated = true;
    updateAABB(entityID);
}
void ComponentList::setAABBSizeZ(double zIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &aabb = registry->get<AABB>(entityID);
    aabb.size.z = zIn;
    aabb.transform.matrixOutdated = true;
    updateAABB(entityID);
}
void ComponentList::setOBBPositionX(double xIn) {
    auto &obb = registry->get<OBB>(mMainWindow->selectedEntity->id());
    obb.position.x = xIn;
}
void ComponentList::setOBBPositionY(double yIn) {
    auto &obb = registry->get<OBB>(mMainWindow->selectedEntity->id());
    obb.position.y = yIn;
}
void ComponentList::setOBBPositionZ(double zIn) {
    auto &obb = registry->get<OBB>(mMainWindow->selectedEntity->id());
    obb.position.z = zIn;
}
void ComponentList::setOBBSizeX(double xIn) {
    auto &obb = registry->get<OBB>(mMainWindow->selectedEntity->id());
    obb.size.x = xIn;
}
void ComponentList::setOBBSizeY(double yIn) {
    auto &obb = registry->get<OBB>(mMainWindow->selectedEntity->id());
    obb.size.y = yIn;
}
void ComponentList::setOBBSizeZ(double zIn) {
    auto &obb = registry->get<OBB>(mMainWindow->selectedEntity->id());
    obb.size.z = zIn;
}
void ComponentList::setSpherePositionX(double xIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &sphere = registry->get<Sphere>(entityID);
    sphere.position.x = xIn;
    sphere.transform.matrixOutdated = true;
    updateSphere(entityID);
}
void ComponentList::setSpherePositionY(double yIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &sphere = registry->get<Sphere>(entityID);
    sphere.position.y = yIn;
    sphere.transform.matrixOutdated = true;
    updateSphere(entityID);
}
void ComponentList::setSpherePositionZ(double zIn) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &sphere = registry->get<Sphere>(entityID);
    sphere.position.z = zIn;
    sphere.transform.matrixOutdated = true;
    updateSphere(entityID);
}
void ComponentList::setSphereRadius(double radius) {
    GLuint entityID = mMainWindow->selectedEntity->id();
    auto &sphere = registry->get<Sphere>(entityID);
    sphere.radius = radius;
    sphere.transform.matrixOutdated = true;
    updateSphere(entityID);
}
void ComponentList::setCylinderPositionX(double xIn) {
    auto &cylinder = registry->get<Sphere>(mMainWindow->selectedEntity->id());
    cylinder.position.x = xIn;
}
void ComponentList::setCylinderPositionY(double yIn) {
    auto &cylinder = registry->get<Sphere>(mMainWindow->selectedEntity->id());
    cylinder.position.y = yIn;
}
void ComponentList::setCylinderPositionZ(double zIn) {
    auto &cylinder = registry->get<Sphere>(mMainWindow->selectedEntity->id());
    cylinder.position.z = zIn;
}
void ComponentList::setCylinderRadius(double radius) {
    auto &cylinder = registry->get<Sphere>(mMainWindow->selectedEntity->id());
    cylinder.radius = radius;
}
void ComponentList::setCylinderHeight(double height) {
    auto &cylinder = registry->get<Sphere>(mMainWindow->selectedEntity->id());
    cylinder.radius = height;
}
// To-do: Either prohibit more than one type of collider or improve this function
void ComponentList::setObjectType(int index) {
    bool isStatic;
    if (index == 0)
        isStatic = true;
    else
        isStatic = false;
    GLuint entityID = mMainWindow->selectedEntity->id();
    if (registry->contains<AABB>(entityID)) {
        auto &aabb = registry->get<AABB>(entityID);
        aabb.isStatic = isStatic;
        return;
    }
    if (registry->contains<Sphere>(entityID)) {
        auto &sphere = registry->get<Sphere>(entityID);
        sphere.isStatic = isStatic;
    }
    if (registry->contains<Cylinder>(entityID)) {
        auto &cylinder = registry->get<Cylinder>(entityID);
        cylinder.isStatic = isStatic;
    }
    if (registry->contains<Plane>(entityID)) {
        auto &plane = registry->get<Plane>(entityID);
        plane.isStatic = isStatic;
    }
}
void ComponentList::setPlaneNormalX(double xIn) {
    auto &plane = registry->get<Plane>(mMainWindow->selectedEntity->id());
    plane.normal.x = xIn;
}
void ComponentList::setPlaneNormalY(double yIn) {
    auto &plane = registry->get<Plane>(mMainWindow->selectedEntity->id());
    plane.normal.y = yIn;
}
void ComponentList::setPlaneNormalZ(double zIn) {
    auto &plane = registry->get<Plane>(mMainWindow->selectedEntity->id());
    plane.normal.z = zIn;
}
void ComponentList::setPlaneDistance(double distance) {
    auto &plane = registry->get<Plane>(mMainWindow->selectedEntity->id());
    plane.distance = distance;
}
QComboBox *ComponentList::makeObjectTypeBox(QGroupBox *objectTypeBox, const Collision &col) {
    QHBoxLayout *objectType = new QHBoxLayout;
    objectType->setMargin(1);
    QComboBox *staticBox = new QComboBox;
    staticBox->addItem("Static");
    staticBox->addItem("Dynamic");
    if (col.isStatic)
        staticBox->setCurrentIndex(0);
    else
        staticBox->setCurrentIndex(1);

    connect(staticBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setObjectType(int)));
    objectType->addWidget(staticBox);
    objectTypeBox->setLayout(objectType);
    return staticBox;
}
/**
 * @brief Utility function for making an XYZ box layout from a given vector
 * @param vector The vector to make the layout from
 * @param layout The layout being created/added to
 * @param minRange Minimum range of the QDoubleSpinBox
 * @param maxRange Maximum range of the QDoubleSpinBox
 * @return Returns a tuple containing the XYZ spinboxes in case you want to plug them into Qt's signal/slot system
 */
std::tuple<QDoubleSpinBox *, QDoubleSpinBox *, QDoubleSpinBox *> ComponentList::makeVectorBox(const vec3 &vector,
                                                                                              QHBoxLayout *layout,
                                                                                              const std::optional<float> &minRange, const std::optional<float> &maxRange) {
    layout->setMargin(1);
    QDoubleSpinBox *xBox, *yBox, *zBox;
    // Set up the Vector Display
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            QLabel *label = new QLabel;
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
            layout->addWidget(label);
        } else {
            switch (i) { // Atm shows relative position if parented to something, global if not. Should probably give the user the option to choose which to show.
            case 1:
                xBox = new QDoubleSpinBox;
                xBox->setDecimals(1);
                xBox->setRange(minRange.value_or(-5000), maxRange.value_or(5000));
                xBox->setMaximumWidth(58);
                xBox->setStyle(fusion);
                xBox->setValue(vector.x);
                layout->addWidget(xBox);
                break;
            case 3:
                yBox = new QDoubleSpinBox;
                yBox->setDecimals(1);
                yBox->setRange(minRange.value_or(-5000), maxRange.value_or(5000));
                yBox->setMaximumWidth(58);
                yBox->setStyle(fusion);
                yBox->setValue(vector.y);
                layout->addWidget(yBox);
                break;
            case 5:
                zBox = new QDoubleSpinBox;
                zBox->setDecimals(1);
                zBox->setRange(minRange.value_or(-5000), maxRange.value_or(5000));
                zBox->setMaximumWidth(58);
                zBox->setStyle(fusion);
                zBox->setValue(vector.z);
                layout->addWidget(zBox);
                break;
            }
        }
    }
    return std::make_tuple(xBox, yBox, zBox);
}
