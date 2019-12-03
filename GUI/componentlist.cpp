#include "componentlist.h"
#include "aisystem.h"
#include "collisionsystem.h"
#include "componentgroupbox.h"
#include "components.h"
#include "constants.h"
#include "customdoublespinbox.h"
#include "customspinbox.h"
#include "hierarchymodel.h"
#include "hierarchyview.h"
#include "inputsystem.h"
#include "mainwindow.h"
#include "movementsystem.h"
#include "particlesystem.h"
#include "rendersystem.h"
#include "verticalscrollarea.h"
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QStyleFactory>
ComponentList::ComponentList(VerticalScrollArea *inScrollArea)
    : registry{Registry::instance()}, scrollArea{inScrollArea} {
    fusion = QStyleFactory::create("fusion");
}
/**
 * @brief When an entity is selected, show all its components in separate groupboxes in the rightmost panel.
 * Order of if-statements is the order the component list will show each component
 */
void ComponentList::setupComponentList() {
    scrollArea->clearLayout();
    GLuint eID{registry->getSelectedEntity()};
    if (registry->contains<Transform>(eID)) {
        setupTransformSettings(registry->get<Transform>(eID));
    }
    if (registry->contains<AIComponent>(eID)) {
        setupAISettings(registry->get<AIComponent>(eID));
    }
    if (registry->contains<AABB>(eID)) {
        setupAABBSettings(registry->get<AABB>(eID));
    }
    if (registry->contains<Sphere>(eID)) {
        setupSphereColliderSettings(registry->get<Sphere>(eID));
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
    if (registry->contains<BSplinePoint>(eID)) {
        setupBSplinePointSettings(registry->get<BSplinePoint>(eID));
    }
    if (registry->contains<GameCamera>(eID)) {
        setupGameCameraSettings(registry->get<GameCamera>(eID));
    }
    if (registry->contains<ParticleEmitter>(eID)) {
        setupEmitterSettings(registry->get<ParticleEmitter>(eID));
    }
}
void ComponentList::addTransformComponent() {
    addComponent<Transform>();
}
void ComponentList::addMaterialComponent() {
    addComponent<Material>();
}
void ComponentList::addMeshComponent() {
    addComponent<Mesh>();
}
void ComponentList::addBSplineComponent() {
    addComponent<BSplinePoint>();
}
void ComponentList::addLightComponent() {
    addComponent<Light>();
}
void ComponentList::addPhysicsComponent() {
    addComponent<Physics>();
}
void ComponentList::addSoundComponent() {
    addComponent<Sound>();
}
void ComponentList::addGameCameraComponent() {
    addComponent<GameCamera>();
}

void ComponentList::addParticleEmitter() {
    addComponent<ParticleEmitter>();
    registry->system<ParticleSystem>()->initEmitter(registry->getSelectedEntity());
}
void ComponentList::addAIComponent() {
    addComponent<AIComponent>();
}

void ComponentList::addAABBCollider() {
    addCollider<AABB>();
    GLuint selectedEntity{registry->getSelectedEntity()};
    if (registry->contains<Transform>(selectedEntity)) {
        auto &trans{registry->get<Transform>(selectedEntity)};
        auto &aabb{registry->get<AABB>(selectedEntity)};
        aabb.size = vec3{trans.localScale.x / 2, trans.localScale.y / 2, trans.localScale.z / 2};
        registry->system<MovementSystem>()->updateAABBTransform(selectedEntity);
    }
}

void ComponentList::addSphereCollider() {
    addCollider<Sphere>();
    GLuint selectedEntity{registry->getSelectedEntity()};

    registry->system<MovementSystem>()->updateSphereTransform(selectedEntity);
}

void ComponentList::addPlaneCollider() {
    addCollider<Plane>();
}
void ComponentList::setupGameCameraSettings(const GameCamera &cam) {
    ComponentGroupBox *box{new ComponentGroupBox{"Game Camera", this}};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);
    QGroupBox *positionBox{new QGroupBox{tr("Position")}};
    positionBox->setStyle(fusion);
    positionBox->setFlat(true);
    QHBoxLayout *position{new QHBoxLayout};
    position->setMargin(1);
    auto [positionX, positionY, positionZ]{makeVectorBox(cam.mCameraPosition, position)};

    InputSystem *inputSys{registry->system<InputSystem>().get()};
    connect(positionX, SIGNAL(valueChanged(double)), inputSys, SLOT(setCameraPositionX(double)));
    connect(positionY, SIGNAL(valueChanged(double)), inputSys, SLOT(setCameraPositionY(double)));
    connect(positionZ, SIGNAL(valueChanged(double)), inputSys, SLOT(setCameraPositionZ(double)));

    positionBox->setLayout(position);
    grid->addWidget(positionBox, 0, 0);

    QGroupBox *rotationBox{new QGroupBox{tr("Camera Rotation")}};
    rotationBox->setAlignment(Qt::AlignCenter);

    rotationBox->setStyle(fusion);
    rotationBox->setFlat(true);

    QHBoxLayout *rotationLayout{new QHBoxLayout};
    rotationLayout->setMargin(1);
    QLabel *pitchLabel{new QLabel(tr("Pitch:"))};
    rotationLayout->addWidget(pitchLabel);
    QDoubleSpinBox *pitch{makeDoubleSpinBox(cam.mPitch, rotationLayout, -180, 180)};
    QLabel *yawLabel{new QLabel(tr("Yaw:"))};
    rotationLayout->addWidget(yawLabel);
    QDoubleSpinBox *yaw{makeDoubleSpinBox(cam.mYaw, rotationLayout, -180, 180)};
    connect(pitch, SIGNAL(valueChanged(double)), inputSys, SLOT(setPitch(double)));
    connect(yaw, SIGNAL(valueChanged(double)), inputSys, SLOT(setYaw(double)));
    rotationBox->setLayout(rotationLayout);
    grid->addWidget(rotationBox, 1, 0);

    // Check box for showing absolute position instead of local position
    QHBoxLayout *check{new QHBoxLayout};
    QPushButton *activeCam{new QPushButton(tr("Is Active"))};
    activeCam->setCheckable(true);
    activeCam->setChecked(cam.mIsActive);
    connect(activeCam, &QPushButton::clicked, inputSys, &InputSystem::setActiveCamera);
    check->addWidget(activeCam);
    grid->addLayout(check, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void ComponentList::setupEmitterSettings(const ParticleEmitter &emitter) {
    ComponentGroupBox *box{new ComponentGroupBox("Particle Emitter", this)};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);

    QGroupBox *boolsBox{new QGroupBox(tr("Particle Data"))};
    boolsBox->setStyle(fusion);
    boolsBox->setFlat(true);
    QHBoxLayout *boolsLayout{new QHBoxLayout};
    boolsLayout->setMargin(1);
    QPushButton *active{new QPushButton(tr("Is Active"))};
    active->setCheckable(true);
    active->setChecked(emitter.isActive);
    QPushButton *decay{new QPushButton(tr("Decay"))};
    decay->setCheckable(true);
    decay->setChecked(emitter.shouldDecay);
    ParticleSystem *particleSys{registry->system<ParticleSystem>().get()};
    connect(active, &QPushButton::clicked, particleSys, &ParticleSystem::setActiveEmitter);
    connect(decay, &QPushButton::clicked, particleSys, &ParticleSystem::setDecayEmitter);
    boolsLayout->addWidget(active);
    boolsLayout->addWidget(decay);
    grid->addLayout(boolsLayout, 0, 0);

    QGroupBox *dirBox{new QGroupBox(tr("Initial Direction"))};
    dirBox->setStyle(fusion);
    dirBox->setFlat(true);
    QHBoxLayout *initDir{new QHBoxLayout};
    initDir->setMargin(1);

    auto [initDirX, initDirY, initDirZ]{makeVectorBox(emitter.initialDirection, initDir)};

    connect(initDirX, SIGNAL(valueChanged(double)), particleSys, SLOT(setInitDirX(double)));
    connect(initDirY, SIGNAL(valueChanged(double)), particleSys, SLOT(setInitDirY(double)));
    connect(initDirZ, SIGNAL(valueChanged(double)), particleSys, SLOT(setInitDirZ(double)));
    dirBox->setLayout(initDir);
    grid->addWidget(dirBox, 1, 0);

    QGroupBox *colorBox{new QGroupBox(tr("Initial Color"))};
    colorBox->setStyle(fusion);
    colorBox->setFlat(true);
    QHBoxLayout *colorLayout{new QHBoxLayout};
    colorLayout->setMargin(1);

    CustomSpinBox *xBox{new CustomSpinBox};
    CustomSpinBox *yBox{new CustomSpinBox};
    CustomSpinBox *zBox{new CustomSpinBox};
    xBox->setRange(0, 255);
    yBox->setRange(0, 255);
    zBox->setRange(0, 255);

    // Set up the Vector Display
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            QLabel *label{new QLabel};
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("R:");
                break;
            case 2:
                label->setText("G:");
                break;
            case 4:
                label->setText("B:");
                break;
            }
            colorLayout->addWidget(label);
        } else {
            switch (i) { // Atm shows relative position if parented to something, global if not. Should probably give the user the option to choose which to show.
            case 1:
                xBox->setValue(emitter.initialColor.red());
                colorLayout->addWidget(xBox);
                break;
            case 3:
                yBox->setValue(emitter.initialColor.green());
                colorLayout->addWidget(yBox);
                break;
            case 5:
                zBox->setValue(emitter.initialColor.blue());
                colorLayout->addWidget(zBox);
                break;
            }
        }
    }

    connect(xBox, SIGNAL(valueChanged(int)), particleSys, SLOT(setInitColorRed(int)));
    connect(yBox, SIGNAL(valueChanged(int)), particleSys, SLOT(setInitColorGreen(int)));
    connect(zBox, SIGNAL(valueChanged(int)), particleSys, SLOT(setInitColorBlue(int)));
    colorBox->setLayout(colorLayout);
    grid->addWidget(colorBox, 2, 0);

    QGroupBox *particlesBox{new QGroupBox(tr("Particle Data"))};
    particlesBox->setStyle(fusion);
    particlesBox->setFlat(true);
    QHBoxLayout *particlesLayout{new QHBoxLayout};
    particlesLayout->setMargin(1);
    CustomSpinBox *particleNumBox{new CustomSpinBox};
    CustomSpinBox *ppsBox{new CustomSpinBox};
    // Set up the Vector Display
    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) {
            QLabel *label{new QLabel};
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("ParticleNr:");
                break;
            case 2:
                label->setText("PPS:");
                break;
            }
            particlesLayout->addWidget(label);
        } else {
            switch (i) {
            case 1:
                particleNumBox->setValue(emitter.numParticles);
                particleNumBox->setRange(1, 1000);
                particlesLayout->addWidget(particleNumBox);
                break;
            case 3:
                ppsBox->setValue(emitter.particlesPerSecond);
                ppsBox->setRange(1, 500);
                particlesLayout->addWidget(ppsBox);
                break;
            }
        }
    }
    connect(particleNumBox, SIGNAL(valueChanged(int)), particleSys, SLOT(setNumParticles(int)));
    connect(ppsBox, SIGNAL(valueChanged(int)), particleSys, SLOT(setPPS(int)));
    particlesBox->setLayout(particlesLayout);
    grid->addWidget(particlesBox, 3, 0);

    QGroupBox *sizeSpeedBox{new QGroupBox(tr("Initial Speed and Size"))};
    sizeSpeedBox->setStyle(fusion);
    sizeSpeedBox->setFlat(true);
    QHBoxLayout *sizeSpeedLayout{new QHBoxLayout};
    sizeSpeedLayout->setMargin(1);
    CustomDoubleSpinBox *speedBox{new CustomDoubleSpinBox};
    CustomDoubleSpinBox *sizeBox{new CustomDoubleSpinBox};
    sizeBox->setSingleStep(0.1);
    // Set up the Vector Display
    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) {
            QLabel *label{new QLabel};
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("Speed:");
                break;
            case 2:
                label->setText("Size:");
                break;
            }
            sizeSpeedLayout->addWidget(label);
        } else {
            switch (i) { // Atm shows relative position if parented to something, global if not. Should probably give the user the option to choose which to show.
            case 1:
                speedBox->setValue(emitter.speed);
                sizeSpeedLayout->addWidget(speedBox);
                break;
            case 3:
                sizeBox->setValue(emitter.size);
                sizeSpeedLayout->addWidget(sizeBox);
                break;
            }
        }
    }
    connect(speedBox, SIGNAL(valueChanged(double)), particleSys, SLOT(setEmitterSpeed(double)));
    connect(sizeBox, SIGNAL(valueChanged(double)), particleSys, SLOT(setEmitterSize(double)));
    sizeSpeedBox->setLayout(sizeSpeedLayout);
    grid->addWidget(sizeSpeedBox, 4, 0);

    QGroupBox *spreadLifeSpanBox{new QGroupBox(tr("Lifespan and Spread"))};
    spreadLifeSpanBox->setStyle(fusion);
    spreadLifeSpanBox->setFlat(true);
    QHBoxLayout *spreadLifeSpanLayout{new QHBoxLayout};
    spreadLifeSpanLayout->setMargin(1);
    CustomDoubleSpinBox *lifespanBox{new CustomDoubleSpinBox};
    CustomDoubleSpinBox *spreadBox{new CustomDoubleSpinBox};
    // Set up the Vector Display
    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) {
            QLabel *label{new QLabel};
            label->setStyle(fusion);
            switch (i) {
            case 0:
                label->setText("Lifespan:");
                break;
            case 2:
                label->setText("Spread:");
                break;
            }
            spreadLifeSpanLayout->addWidget(label);
        } else {
            switch (i) {
            case 1:
                lifespanBox->setValue(emitter.initLifeSpan);
                spreadLifeSpanLayout->addWidget(lifespanBox);
                break;
            case 3:
                spreadBox->setValue(emitter.spread);
                spreadLifeSpanLayout->addWidget(spreadBox);
                break;
            }
        }
    }
    connect(lifespanBox, SIGNAL(valueChanged(double)), particleSys, SLOT(setEmitterLifeSpan(double)));
    connect(spreadBox, SIGNAL(valueChanged(double)), particleSys, SLOT(setEmitterSpread(double)));
    spreadLifeSpanBox->setLayout(spreadLifeSpanLayout);
    grid->addWidget(spreadLifeSpanBox, 5, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupBSplinePointSettings(const BSplinePoint &point) {
    ComponentGroupBox *box{new ComponentGroupBox("BSpline Point", this)};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);
    QGroupBox *locationBox{new QGroupBox(tr("Location"))};
    locationBox->setStyle(fusion);
    locationBox->setFlat(true);
    QHBoxLayout *location{new QHBoxLayout};
    location->setMargin(1);
    auto [locationX, locationY, locationZ]{makeVectorBox(point.location, location)};
    AISystem *aiSys{registry->system<AISystem>().get()};
    connect(locationX, SIGNAL(valueChanged(double)), aiSys, SLOT(setBSPlinePointX(double)));
    connect(locationY, SIGNAL(valueChanged(double)), aiSys, SLOT(setBSPlinePointY(double)));
    connect(locationZ, SIGNAL(valueChanged(double)), aiSys, SLOT(setBSPlinePointZ(double)));

    locationBox->setLayout(location);
    grid->addWidget(locationBox, 0, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupAABBSettings(const AABB &col) {
    ComponentGroupBox *box{new ComponentGroupBox("AABB Collider", this)};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);
    QGroupBox *originBox{new QGroupBox(tr("Origin"))};
    originBox->setStyle(fusion);
    originBox->setFlat(true);
    QHBoxLayout *origin{new QHBoxLayout};
    origin->setMargin(1);
    auto [originX, originY, originZ]{makeVectorBox(col.origin, origin)}; // Not sure yet if this is something that's supposed to be updated
    connect(this, &ComponentList::originX, originX, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::originY, originY, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::originZ, originZ, &QDoubleSpinBox::setValue);

    CollisionSystem *colSys{registry->system<CollisionSystem>().get()};
    connect(originX, SIGNAL(valueChanged(double)), colSys, SLOT(setOriginX(double)));
    connect(originY, SIGNAL(valueChanged(double)), colSys, SLOT(setOriginY(double)));
    connect(originZ, SIGNAL(valueChanged(double)), colSys, SLOT(setOriginZ(double)));

    originBox->setLayout(origin);
    grid->addWidget(originBox, 0, 0);

    QGroupBox *hSizeBox{new QGroupBox(tr("Half-Size"))};
    hSizeBox->setStyle(fusion);
    hSizeBox->setFlat(true);

    QHBoxLayout *hSize{new QHBoxLayout};
    hSize->setMargin(1);

    auto [sizeX, sizeY, sizeZ]{makeVectorBox(col.size, hSize, 0.1f, 5000)};
    connect(this, &ComponentList::AABBSizeX, sizeX, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::AABBSizeY, sizeY, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::AABBSizeZ, sizeZ, &QDoubleSpinBox::setValue);
    connect(sizeX, SIGNAL(valueChanged(double)), colSys, SLOT(setAABBSizeX(double)));
    connect(sizeY, SIGNAL(valueChanged(double)), colSys, SLOT(setAABBSizeY(double)));
    connect(sizeZ, SIGNAL(valueChanged(double)), colSys, SLOT(setAABBSizeZ(double)));

    hSizeBox->setLayout(hSize);
    grid->addWidget(hSizeBox, 1, 0);

    QGroupBox *objectTypeBox{new QGroupBox(tr("Object Type"))};
    makeObjectTypeBox(objectTypeBox, col);
    grid->addWidget(objectTypeBox, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void ComponentList::setupSphereColliderSettings(const Sphere &col) {
    ComponentGroupBox *box{new ComponentGroupBox("Sphere Collider", this)};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);
    QGroupBox *posBox{new QGroupBox(tr("Position"))};
    posBox->setStyle(fusion);
    posBox->setFlat(true);
    QHBoxLayout *position{new QHBoxLayout};
    position->setMargin(1);
    auto [positionX, positionY, positionZ]{makeVectorBox(col.position, position)};
    connect(this, &ComponentList::spherePositionX, positionX, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::spherePositionY, positionY, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::spherePositionZ, positionZ, &QDoubleSpinBox::setValue);

    CollisionSystem *colSys{registry->system<CollisionSystem>().get()};
    connect(positionX, SIGNAL(valueChanged(double)), colSys, SLOT(setSpherePositionX(double)));
    connect(positionY, SIGNAL(valueChanged(double)), colSys, SLOT(setSpherePositionY(double)));
    connect(positionZ, SIGNAL(valueChanged(double)), colSys, SLOT(setSpherePositionZ(double)));
    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    QGroupBox *radiusBox{new QGroupBox(tr("Radius"))};
    radiusBox->setStyle(fusion);
    radiusBox->setFlat(true);

    QHBoxLayout *radiusLayout{new QHBoxLayout};
    radiusLayout->setMargin(1);
    QDoubleSpinBox *radius{makeDoubleSpinBox(col.radius, radiusLayout)};
    connect(this, &ComponentList::sphereRadius, radius, &QDoubleSpinBox::setValue);
    connect(radius, SIGNAL(valueChanged(double)), colSys, SLOT(setSphereRadius(double)));

    radiusBox->setLayout(radiusLayout);
    grid->addWidget(radiusBox, 1, 0);

    QGroupBox *objectTypeBox{new QGroupBox(tr("Object Type"))};
    makeObjectTypeBox(objectTypeBox, col);
    grid->addWidget(objectTypeBox, 2, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void ComponentList::setupPlaneColliderSettings(const Plane &col) {
    ComponentGroupBox *box{new ComponentGroupBox("Plane Collider", this)};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);
    QGroupBox *normalBox{new QGroupBox(tr("Normal"))};
    normalBox->setStyle(fusion);
    normalBox->setFlat(true);
    QHBoxLayout *normal{new QHBoxLayout};
    normal->setMargin(1);
    auto [normalX, normalY, normalZ]{makeVectorBox(col.normal, normal)};

    connect(this, &ComponentList::planeNormalX, normalX, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::planeNormalY, normalY, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::planeNormalZ, normalZ, &QDoubleSpinBox::setValue);

    CollisionSystem *colSys{registry->system<CollisionSystem>().get()};
    connect(normalX, SIGNAL(valueChanged(double)), colSys, SLOT(setPlaneNormalX(double)));
    connect(normalY, SIGNAL(valueChanged(double)), colSys, SLOT(setPlaneNormalY(double)));
    connect(normalZ, SIGNAL(valueChanged(double)), colSys, SLOT(setPlaneNormalZ(double)));

    normalBox->setLayout(normal);
    grid->addWidget(normalBox, 0, 0);

    QGroupBox *distanceBox{new QGroupBox(tr("Distance"))};
    distanceBox->setStyle(fusion);
    distanceBox->setFlat(true);

    QHBoxLayout *distanceLayout{new QHBoxLayout};
    distanceLayout->setMargin(1);
    QDoubleSpinBox *distance{makeDoubleSpinBox(col.distance, distanceLayout)};
    connect(this, &ComponentList::planeDistance, distance, &QDoubleSpinBox::setValue);
    connect(distance, SIGNAL(valueChanged(double)), colSys, SLOT(setPlaneDistance(double)));

    distanceBox->setLayout(distanceLayout);
    grid->addWidget(distanceBox, 1, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupAISettings(const AIComponent &ai) {
    ComponentGroupBox *box{new ComponentGroupBox("AI", this)};
    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);

    QGroupBox *hpBox{new QGroupBox(tr("Health"))};
    hpBox->setStyle(fusion);
    hpBox->setFlat(true);

    QHBoxLayout *hpLayout{new QHBoxLayout};
    hpLayout->setMargin(1);
    QSpinBox *health{new QSpinBox};
    health->setRange(0, 5000);
    health->setMaximumWidth(58);
    health->setStyle(fusion);
    health->setValue(ai.health);
    hpLayout->addWidget(health);
    connect(this, &ComponentList::changeHealth, health, &QSpinBox::setValue);
    AISystem *aiSys{registry->system<AISystem>().get()};
    connect(health, SIGNAL(valueChanged(int)), aiSys, SLOT(setHealth(int)));

    hpBox->setLayout(hpLayout);
    grid->addWidget(hpBox, 0, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupMeshSettings(const Mesh &mesh) {
    ComponentGroupBox *box{new ComponentGroupBox("Mesh", this)};

    objFileLabel = new QLabel(box);
    objFileLabel->setText(ResourceManager::instance()->getMeshName(mesh));
    QPushButton *button{new QPushButton("Change Mesh")};
    connect(button, &QPushButton::clicked, this, &ComponentList::setNewMesh);

    QHBoxLayout *meshLayout{new QHBoxLayout};
    meshLayout->setMargin(1);
    meshLayout->addWidget(objFileLabel);
    meshLayout->addWidget(button);
    box->setLayout(meshLayout);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupMaterialSettings(const Material &mat) {
    ComponentGroupBox *box{new ComponentGroupBox("Material", this)};

    QVBoxLayout *shader{new QVBoxLayout};
    shader->setMargin(1);
    QComboBox *shaderType{new QComboBox};
    ResourceManager *factory{ResourceManager::instance()};
    for (auto type : factory->getShaders()) {
        QString curText{QString::fromStdString(type.second->getName())};
        shaderType->addItem(curText);
        if (curText == QString::fromStdString(mat.mShader->getName()))
            shaderType->setCurrentIndex(shaderType->findText(curText));
    }
    RenderSystem *renderSys{registry->system<RenderSystem>().get()};
    connect(shaderType, SIGNAL(currentIndexChanged(const QString &)), renderSys, SLOT(changeShader(const QString &)));

    QString curTexture{factory->getTextureName(mat.mTextureUnit)};
    QLabel *textureThumb{new QLabel(box)};
    QPixmap thumbNail;
    thumbNail.load(QString::fromStdString(gsl::assetFilePath) + "Textures/" + curTexture); // Load the texture image into the pixmap
    if (!thumbNail.isNull()) {
        textureThumb->setPixmap(thumbNail.scaled(18, 18)); // Get a scaled version of the image loaded above
    }
    texFileLabel = new QLabel(box);
    texFileLabel->setText(curTexture); // Saves the file name of the texture image
    QPushButton *browseImages{new QPushButton("Browse", this)};
    browseImages->setStyle(fusion);
    connect(browseImages, &QPushButton::clicked, this, &ComponentList::setNewTextureFile);
    QHBoxLayout *texture{new QHBoxLayout};
    texture->addWidget(textureThumb);
    texture->addWidget(texFileLabel);
    texture->addWidget(browseImages);

    colorLabel = new QLabel;
    QPixmap curColor{18, 18};
    vec3 oColor{mat.mObjectColor};
    rgb.setRgbF(oColor.x, oColor.y, oColor.z); // setRgbF takes floats in the 0-1 range, which is what we want
    curColor.fill(rgb);
    colorLabel->setPixmap(curColor);
    QPushButton *colorButton{new QPushButton(tr("Change Color"))};
    connect(colorButton, &QPushButton::clicked, this, &ComponentList::setColor);

    QHBoxLayout *color{new QHBoxLayout};
    color->addWidget(colorLabel);
    color->addWidget(colorButton);

    shader->addWidget(shaderType);
    shader->addLayout(texture);
    shader->addLayout(color);
    box->setLayout(shader);
    scrollArea->addGroupBox(box);
}
void ComponentList::setupTransformSettings(const Transform &trans) {
    ComponentGroupBox *box{new ComponentGroupBox("Transform", this)};

    QGridLayout *grid{new QGridLayout};
    grid->setMargin(2);

    MovementSystem *movement{registry->system<MovementSystem>().get()};
    connect(movement, &MovementSystem::positionChanged, this, &ComponentList::updatePositionVals);
    connect(movement, &MovementSystem::rotationChanged, this, &ComponentList::updateRotationVals);
    connect(movement, &MovementSystem::scaleChanged, this, &ComponentList::updateScaleVals);

    // Set up the Position display
    QGroupBox *posBox{new QGroupBox(tr("Position"))};
    posBox->setStyle(fusion);
    posBox->setFlat(true);
    QHBoxLayout *position{new QHBoxLayout};
    position->setMargin(1);
    std::tie(xVal, yVal, zVal) = makeVectorBox(trans.localPosition, position);
    connect(this, &ComponentList::posX, xVal, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::posY, yVal, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::posZ, zVal, &QDoubleSpinBox::setValue);

    connect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
    connect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
    connect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
    posBox->setLayout(position);
    grid->addWidget(posBox, 0, 0);

    // Check box for showing absolute position instead of local position
    QHBoxLayout *check{new QHBoxLayout};
    abs = new QCheckBox(tr("Absolute Position"));
    connect(abs, &QCheckBox::stateChanged, this, &ComponentList::updatePosSpinBoxes);
    check->addWidget(abs);
    grid->addLayout(check, 1, 0);

    // Set up the Rotation Display
    QGroupBox *rotBox{new QGroupBox(tr("Rotation"))};
    rotBox->setStyle(fusion);
    rotBox->setFlat(true);
    QHBoxLayout *rotation{new QHBoxLayout};
    rotation->setMargin(1);
    auto [rotX, rotY, rotZ]{makeVectorBox(trans.localRotation, rotation, -180, 180)};
    connect(this, &ComponentList::rotX, rotX, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::rotY, rotY, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::rotZ, rotZ, &QDoubleSpinBox::setValue);

    connect(rotX, SIGNAL(valueChanged(double)), this, SLOT(setRotationX(double)));
    connect(rotY, SIGNAL(valueChanged(double)), this, SLOT(setRotationY(double)));
    connect(rotZ, SIGNAL(valueChanged(double)), this, SLOT(setRotationZ(double)));
    rotBox->setLayout(rotation);
    grid->addWidget(rotBox, 2, 0);

    // Set up the Rotation Display
    QGroupBox *scaleBox{new QGroupBox(tr("Scale"))};
    scaleBox->setStyle(fusion);
    scaleBox->setFlat(true);
    QHBoxLayout *scale{new QHBoxLayout(box)};
    scale->setMargin(1);
    auto [scaleX, scaleY, scaleZ]{makeVectorBox(trans.localScale, scale, 0.1, 500)};
    connect(this, &ComponentList::scaleX, scaleX, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::scaleY, scaleY, &QDoubleSpinBox::setValue);
    connect(this, &ComponentList::scaleZ, scaleZ, &QDoubleSpinBox::setValue);

    connect(scaleX, SIGNAL(valueChanged(double)), this, SLOT(setScaleX(double)));
    connect(scaleY, SIGNAL(valueChanged(double)), this, SLOT(setScaleY(double)));
    connect(scaleZ, SIGNAL(valueChanged(double)), this, SLOT(setScaleZ(double)));
    scaleBox->setLayout(scale);
    grid->addWidget(scaleBox, 3, 0);

    box->setLayout(grid);
    scrollArea->addGroupBox(box);
}

void ComponentList::setNewTextureFile() {
    QString directory{QString::fromStdString(gsl::assetFilePath) + "Textures"};
    QString fileName{QFileDialog::getOpenFileName(this, tr("Open Image"), directory,
                                                  tr("Image Files (*.bmp *.jpg *.png)"))};
    if (!fileName.isEmpty()) {
        QFileInfo file{fileName};
        fileName = file.fileName();
        ResourceManager *factory{ResourceManager::instance()};
        factory->loadTexture(fileName.toStdString());
        registry->get<Material>(registry->getSelectedEntity()).mTextureUnit = factory->getTexture(fileName.toStdString())->textureUnit();
        texFileLabel->setText(fileName);
    }
}
void ComponentList::setNewMesh() {
    QString directory{QString::fromStdString(gsl::assetFilePath) + "Meshes"};
    QString fileName{QFileDialog::getOpenFileName(this, tr("Open Mesh File"), directory,
                                                  tr("OBJ Files (*.obj)"))};
    if (!fileName.isEmpty()) {
        QFileInfo file{fileName};
        fileName = file.fileName();
        ResourceManager *factory{ResourceManager::instance()};
        factory->setMesh(fileName.toStdString(), registry->getSelectedEntity());
        objFileLabel->setText(fileName);
    }
}
/**
 * @brief Not sure why this gives a geometry warning, ask Ole?
 */
void ComponentList::setColor() {
    const QColor color{QColorDialog::getColor(rgb, this, "Select Color")};
    if (color.isValid()) {
        QPixmap newRgb{18, 18};
        newRgb.fill(color);
        colorLabel->setPixmap(newRgb);
    }
    registry->get<Material>(registry->getSelectedEntity()).mObjectColor = vec3{static_cast<GLfloat>(color.redF()), static_cast<GLfloat>(color.greenF()), static_cast<GLfloat>(color.blueF())};
}

void ComponentList::updatePosSpinBoxes(int state) {
    disconnect(xVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionX(double)));
    disconnect(yVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionY(double)));
    disconnect(zVal, SIGNAL(valueChanged(double)), this, SLOT(setPositionZ(double)));
    GLuint entityID{registry->getSelectedEntity()};
    auto &trans{registry->get<Transform>(entityID)};
    switch (state) {
    case 0:
        xVal->setValue(trans.localPosition.x);
        yVal->setValue(trans.localPosition.y);
        zVal->setValue(trans.localPosition.z);
        break;
    case 2:
        registry->system<MovementSystem>()->getAbsolutePosition(entityID); // have to call this function once to update the global pos variable if it hasn't been cached yet
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
    if (eID == registry->getSelectedEntity()) {
        emit posX(newPos.x);
        emit posY(newPos.y);
        emit posZ(newPos.z);
    }
}
void ComponentList::updateRotationVals(GLuint eID, vec3 newRot) {
    if (eID == registry->getSelectedEntity()) {
        emit rotX(newRot.x);
        emit rotY(newRot.y);
        emit rotZ(newRot.z);
    }
}
void ComponentList::updateScaleVals(GLuint eID, vec3 newScale) {
    if (eID == registry->getSelectedEntity()) {
        emit scaleX(newScale.x);
        emit scaleY(newScale.y);
        emit scaleZ(newScale.z);
    }
}

void ComponentList::setPositionX(double xIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    if (abs->isChecked())
        movement->setAbsolutePositionX(entityID, xIn, false);
    else
        movement->setLocalPositionX(entityID, xIn, false);

    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setPositionY(double yIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    if (abs->isChecked())
        movement->setAbsolutePositionY(entityID, yIn, false);
    else
        movement->setLocalPositionY(entityID, yIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setPositionZ(double zIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    if (abs->isChecked())
        movement->setAbsolutePositionZ(entityID, zIn, false);
    else
        movement->setLocalPositionZ(entityID, zIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setRotationX(double xIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    movement->setRotationX(entityID, xIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setRotationY(double yIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    movement->setRotationY(entityID, yIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setRotationZ(double zIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    movement->setRotationZ(entityID, zIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setScaleX(double xIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    movement->setScaleX(entityID, xIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setScaleY(double yIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    movement->setScaleY(entityID, yIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}
void ComponentList::setScaleZ(double zIn) {
    auto movement{registry->system<MovementSystem>()};
    GLuint entityID{registry->getSelectedEntity()};
    movement->setScaleZ(entityID, zIn, false);
    if (!ResourceManager::instance()->isPlaying())
        movement->updateEntity(entityID);
}

QComboBox *ComponentList::makeObjectTypeBox(QGroupBox *objectTypeBox, const Collision &col) {
    QHBoxLayout *objectType{new QHBoxLayout};
    objectType->setMargin(1);
    QComboBox *staticBox{new QComboBox};
    staticBox->addItem("Static");
    staticBox->addItem("Dynamic");
    if (col.isStatic)
        staticBox->setCurrentIndex(0);
    else
        staticBox->setCurrentIndex(1);

    CollisionSystem *colSys{registry->system<CollisionSystem>().get()};
    connect(staticBox, SIGNAL(currentIndexChanged(int)), colSys, SLOT(setObjectType(int)));
    objectType->addWidget(staticBox);
    objectTypeBox->setLayout(objectType);
    return staticBox;
}
CustomDoubleSpinBox *ComponentList::makeDoubleSpinBox(const double &num, QHBoxLayout *layout,
                                                      const std::optional<float> &minRange, const std::optional<float> &maxRange) {
    layout->setMargin(1);
    CustomDoubleSpinBox *distance{new CustomDoubleSpinBox(minRange, maxRange)};
    distance->setValue(num);
    layout->addWidget(distance);

    return distance;
}
/**
 * @brief Utility function for making an XYZ box layout from a given vector
 * @param vector The vector to make the layout from
 * @param layout The layout being created/added to
 * @param minRange Minimum range of the QDoubleSpinBox
 * @param maxRange Maximum range of the QDoubleSpinBox
 * @return Returns a tuple containing the XYZ spinboxes in case you want to plug them into Qt's signal/slot system
 */
std::tuple<CustomDoubleSpinBox *, CustomDoubleSpinBox *, CustomDoubleSpinBox *> ComponentList::makeVectorBox(const vec3 &vector,
                                                                                                             QHBoxLayout *layout,
                                                                                                             const std::optional<float> &minRange, const std::optional<float> &maxRange) {
    layout->setMargin(1);

    CustomDoubleSpinBox *xBox{new CustomDoubleSpinBox(minRange, maxRange)};
    CustomDoubleSpinBox *yBox{new CustomDoubleSpinBox(minRange, maxRange)};
    CustomDoubleSpinBox *zBox{new CustomDoubleSpinBox(minRange, maxRange)};

    // Set up the Vector Display
    for (int i = 0; i < 6; i++) {
        if (i % 2 == 0) {
            QLabel *label{new QLabel};
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
                xBox->setValue(vector.x);
                layout->addWidget(xBox);
                break;
            case 3:
                yBox->setValue(vector.y);
                layout->addWidget(yBox);
                break;
            case 5:
                zBox->setValue(vector.z);
                layout->addWidget(zBox);
                break;
            }
        }
    }
    return {xBox, yBox, zBox};
}
