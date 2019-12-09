#include "scene.h"
#include "cameracontroller.h"
#include "constants.h"
#include "core.h"
#include "inputsystem.h"
#include "movementsystem.h"
#include "phongshader.h"
#include "registry.h"
#include "resourcemanager.h"
#include "shader.h"
#include "skyboxshader.h"
#include "textureshader.h"
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rendersystem.h>
#include <sstream>

void Scene::saveScene(const QString &fileName)
{
    if (fileName.isEmpty())
        return;

    Registry *registry = Registry::instance();
    const std::vector<GLuint> &entities{registry->getEntities()};
    auto inputSys{registry->system<InputSystem>()};

    StringBuffer buf;
    PrettyWriter<StringBuffer> writer{buf};

    mName = fileName;
    writer.StartObject();

    for (auto &entity : entities) {
        const EInfo info{registry->get<EInfo>(entity)};

        if (entity != 0 && !info.name.isEmpty()) { // Ignore the first entity, it's reserved for the XYZ lines. (Hardcoded in RenderWindow to be loaded before loadProject, so it's always first)
            writer.String("Entity");
            writer.StartObject();
            writer.Key("name");
            writer.String(info.name.toStdString().c_str());
            writer.Key("id");
            writer.Uint(entity);
            writer.Key("components");
            writer.StartObject();
            if (registry->contains<Transform>(entity)) {
                const Transform trans{registry->get<Transform>(entity)};
                writer.Key("transform");
                writer.StartObject();

                writer.Key("position");
                writer.StartArray();
                writer.Double(trans.localPosition.x);
                writer.Double(trans.localPosition.y);
                writer.Double(trans.localPosition.z);
                writer.EndArray();

                writer.Key("rotation");
                writer.StartArray();
                writer.Double(trans.localRotation.x);
                writer.Double(trans.localRotation.y);
                writer.Double(trans.localRotation.z);
                writer.EndArray();

                writer.Key("scale");
                writer.StartArray();
                writer.Double(trans.localScale.x);
                writer.Double(trans.localScale.y);
                writer.Double(trans.localScale.z);
                writer.EndArray();

                writer.Key("parent");
                writer.Int(trans.parentID);

                if (!trans.children.empty()) {
                    writer.Key("children");
                    writer.StartArray();
                    for (auto child : trans.children) {
                        writer.Int(child);
                    }
                    writer.EndArray();
                }
                writer.EndObject();
            }
            if (registry->contains<Material>(entity)) {
                writer.Key("material");
                writer.StartObject();
                const Material &mat{registry->get<Material>(entity)};

                writer.Key("color");
                writer.StartArray();
                writer.Double(mat.mObjectColor.x);
                writer.Double(mat.mObjectColor.y);
                writer.Double(mat.mObjectColor.z);
                writer.EndArray();

                writer.Key("specstr");
                writer.Double(mat.mSpecularStrength);
                writer.Key("specexp");
                writer.Int(mat.mSpecularExponent);

                writer.Key("textureunit");
                writer.Int(mat.mTextureUnit);

                writer.Key("shader");
                writer.String(mat.mShader->getName().c_str());

                writer.EndObject();
            }
            if (registry->contains<Mesh>(entity)) {
                writer.Key("mesh");
                writer.StartObject();
                const Mesh &mesh{registry->get<Mesh>(entity)};
                //                if (mesh.mName == "")
                //                    qDebug() << "No mesh name!";
                writer.Key("name");
                writer.String(mesh.mName.c_str()); // Use the mesh name (either a prefab or a file in Assets/Meshes) to find out what to do from here.
                writer.EndObject();
            }
            if (registry->contains<Light>(entity)) {
                writer.Key("light");
                writer.StartObject();
                const Light &light{registry->get<Light>(entity)};

                writer.Key("ambstr");
                writer.Double(light.mAmbientStrength);

                writer.Key("ambcolor");
                writer.StartArray();
                writer.Double(light.mAmbientColor.x);
                writer.Double(light.mAmbientColor.y);
                writer.Double(light.mAmbientColor.z);
                writer.EndArray();

                writer.Key("lightstr");
                writer.Double(light.mLightStrength);

                writer.Key("lightcolor");
                writer.StartArray();
                writer.Double(light.mLightColor.x);
                writer.Double(light.mLightColor.y);
                writer.Double(light.mLightColor.z);
                writer.EndArray();

                writer.Key("color");
                writer.StartArray();
                writer.Double(light.mObjectColor.x);
                writer.Double(light.mObjectColor.y);
                writer.Double(light.mObjectColor.z);
                writer.EndArray();

                writer.EndObject();
            }
            if (registry->contains<BillBoard>(entity)) {
                const BillBoard &board{registry->get<BillBoard>(entity)};
                writer.Key("billboard");
                writer.StartObject();
                writer.Key("y-up");
                writer.Bool(board.mConstantYUp);
                writer.Key("normalversion");
                writer.Bool(board.mNormalVersion);
                writer.EndObject();
            }
            if (registry->contains<Sound>(entity)) {
                const Sound &sound{registry->get<Sound>(entity)};
                writer.Key("sound");
                writer.StartObject();
                writer.Key("filename");
                writer.String(sound.name.c_str());
                writer.Key("loop");
                writer.Bool(sound.looping);
                writer.Key("gain");
                writer.Double(sound.gain);
                writer.EndObject();
            }
            if (registry->contains<ParticleEmitter>(entity)) {
                const ParticleEmitter &emitter{registry->get<ParticleEmitter>(entity)};
                writer.Key("particleemitter");
                writer.StartObject();
                writer.Key("isactive");
                writer.Bool(emitter.isActive);
                writer.Key("decay");
                writer.Bool(emitter.shouldDecay);

                writer.Key("numparticles");
                writer.Int(emitter.numParticles);
                writer.Key("pps");
                writer.Int(emitter.particlesPerSecond);

                writer.Key("initdir");
                writer.StartArray();
                writer.Double(emitter.initialDirection.x);
                writer.Double(emitter.initialDirection.y);
                writer.Double(emitter.initialDirection.z);
                writer.EndArray();

                writer.Key("initColor");
                writer.StartArray();
                writer.Int(emitter.initialColor.red());
                writer.Int(emitter.initialColor.green());
                writer.Int(emitter.initialColor.blue());
                writer.Int(emitter.initialColor.alpha());
                writer.EndArray();

                writer.Key("textureunit");
                writer.Int(emitter.textureUnit);

                writer.Key("speed");
                writer.Double(emitter.speed);
                writer.Key("size");
                writer.Double(emitter.size);
                writer.Key("spread");
                writer.Double(emitter.spread);
                writer.Key("lifespan");
                writer.Double(emitter.lifeSpan);
                writer.EndObject();
            }
            if (registry->contains<AABB>(entity)) {
                writer.Key("AABB");
                writer.StartObject();
                const AABB &aabb{registry->get<AABB>(entity)};

                writer.Key("origin");
                writer.StartArray();
                writer.Double(aabb.origin.x);
                writer.Double(aabb.origin.y);
                writer.Double(aabb.origin.z);
                writer.EndArray();
                writer.Key("size");
                writer.StartArray();
                writer.Double(aabb.size.x);
                writer.Double(aabb.size.y);
                writer.Double(aabb.size.z);
                writer.EndArray();
                writer.Key("static");
                writer.Bool(aabb.isStatic);
                writer.EndObject();
            }
            if (registry->contains<Sphere>(entity)) {
                writer.Key("Sphere");
                writer.StartObject();
                const Sphere &sphere{registry->get<Sphere>(entity)};

                writer.Key("position");
                writer.StartArray();
                writer.Double(sphere.position.x);
                writer.Double(sphere.position.y);
                writer.Double(sphere.position.z);
                writer.EndArray();
                writer.Key("radius");
                writer.Double(sphere.radius);
                writer.Key("static");
                writer.Bool(sphere.isStatic);
                writer.EndObject();
            }
            if (registry->contains<AIComponent>(entity)) {
                writer.Key("AI");
                writer.StartObject();
                const AIComponent &ai{registry->get<AIComponent>(entity)};

                writer.Key("health");
                writer.Int(ai.health);
                writer.Key("speed");
                writer.Double(ai.moveSpeed);
                writer.EndObject();
            }
            if (registry->contains<Buildable>(entity)) {
                writer.Key("Buildable");
                writer.StartObject();
                const Buildable &build{registry->get<Buildable>(entity)};
                writer.Key("canBuild");
                writer.Bool(build.isBuildable);
                writer.EndObject();
            }
            if (registry->contains<BSplinePoint>(entity)) {
                writer.Key("BSplinePoint");
                writer.StartObject();
                const BSplinePoint &cp{registry->get<BSplinePoint>(entity)};
                writer.Key("location");
                writer.StartArray();
                writer.Double(cp.location.x);
                writer.Double(cp.location.y);
                writer.Double(cp.location.z);
                writer.EndArray();
                writer.EndObject();
            }
            if (registry->contains<GameCamera>(entity)) {
                writer.Key("GameCamera");
                writer.StartObject();
                const GameCamera &cam{registry->get<GameCamera>(entity)};
                writer.Key("position");
                writer.StartArray();
                writer.Double(cam.mCameraPosition.x);
                writer.Double(cam.mCameraPosition.y);
                writer.Double(cam.mCameraPosition.z);
                writer.EndArray();
                writer.Key("pitch");
                writer.Double(cam.mPitch);
                writer.Key("yaw");
                writer.Double(cam.mYaw);
                writer.Key("active");
                writer.Bool(cam.mIsActive);
                writer.EndObject();
            }

            writer.EndObject();
            writer.EndObject();
        }
    }

    writer.EndObject();
    std::ofstream of{gsl::sceneFilePath + fileName.toStdString() + ".json"};
    of << buf.GetString();
    if (!of.good() || !of)
        throw std::runtime_error("Can't write the JSON string to the file!");
}
void Scene::loadScene(const QString &fileName)
{
    if (fileName.isEmpty()) {
        return;
    }
    Registry *registry{Registry::instance()};
    ResourceManager *factory{ResourceManager::instance()};
    factory->setLoading(true);
    registry->clearScene();
    auto search = mScenes.find(fileName);
    if (search == mScenes.end())
        loadSceneFromFile(fileName);
    else {
        Document sceneDoc;
        sceneDoc.Parse(mScenes[fileName].c_str());
        populateScene(sceneDoc);
    }
    mName = fileName.chopped(5);
    registry->updateChildParent();
    factory->setLoading(false);
}
void Scene::populateScene(const Document &scene)
{
    std::map<int, int> parentID;
    std::map<int, int> idPairs;
    Registry *registry{Registry::instance()};
    ResourceManager *factory{ResourceManager::instance()};
    if (!scene.HasMember("Entity"))
        return;
    // Iterate through each entity in the scene
    for (Value::ConstMemberIterator itr = scene.MemberBegin(); itr != scene.MemberEnd(); ++itr) {
        GLuint id{registry->makeEntity(itr->value["name"].GetString())};
        idPairs[itr->value["id"].GetInt()] = id;
        // Iterate through each of the members in the entity (name, id, components)
        for (Value::ConstMemberIterator comp{itr->value["components"].MemberBegin()}; comp != itr->value["components"].MemberEnd(); ++comp) {
            if (comp->name == "transform") {
                vec3 position{comp->value["position"][0].GetFloat(), comp->value["position"][1].GetFloat(), comp->value["position"][2].GetFloat()};
                vec3 rotation{comp->value["rotation"][0].GetFloat(), comp->value["rotation"][1].GetFloat(), comp->value["rotation"][2].GetFloat()};
                vec3 scale{comp->value["scale"][0].GetFloat(), comp->value["scale"][1].GetFloat(), comp->value["scale"][2].GetFloat()};
                registry->add<Transform>(id, position, rotation, scale);
                if (comp->value["parent"].GetInt() != -1) {
                    if (idPairs.find(comp->value["parent"].GetInt()) != idPairs.end())                   // if the parent has been parsed already
                        registry->get<Transform>(id).parentID = idPairs[comp->value["parent"].GetInt()]; // use the old parentID to set the new parentID
                    else
                        parentID[id] = comp->value["parent"].GetInt(); // if not, add it to the list of parent/child pairs that must be updated after the loop
                }
                registry->system<MovementSystem>()->updateEntity(id); // Note: Not sure about this line, but for now it should ensure that all transforms are correct as soon as they are created
            }
            else if (comp->name == "material") {
                vec3 color{comp->value["color"][0].GetFloat(), comp->value["color"][1].GetFloat(), comp->value["color"][2].GetFloat()};
                GLfloat specStr{comp->value["specstr"].GetFloat()};
                GLint specExp{comp->value["specexp"].GetInt()};
                QString shaderName{comp->value["shader"].GetString()};
                cjk::Ref<Shader> shader{nullptr};
                ResourceManager *factory{ResourceManager::instance()};
                if (shaderName == "PlainShader")
                    shader = factory->getShader<ColorShader>();
                else if (shaderName == "TextureShader")
                    shader = factory->getShader<TextureShader>();
                else if (shaderName == "PhongShader")
                    shader = factory->getShader<PhongShader>();
                else if (shaderName == "SkyboxShader")
                    shader = factory->getShader<SkyboxShader>();
                registry->add<Material>(id, shader, comp->value["textureunit"].GetUint(), color, specStr, specExp);
            }
            else if (comp->name == "mesh") {
                std::string meshName{comp->value["name"].GetString()};
                factory->addMeshComponent(meshName, id);
                if (meshName == "Skybox")
                    registry->system<RenderSystem>()->setSkyBoxID(id);
            }
            else if (comp->name == "light") { // Again, temporary, very static functionality atm
                GLfloat ambStr{comp->value["ambstr"].GetFloat()};
                vec3 ambColor{comp->value["ambcolor"][0].GetFloat(), comp->value["ambcolor"][1].GetFloat(), comp->value["ambcolor"][2].GetFloat()};
                GLfloat lightStr{comp->value["lightstr"].GetFloat()};
                vec3 lightColor{comp->value["lightcolor"][0].GetFloat(), comp->value["lightcolor"][1].GetFloat(), comp->value["lightcolor"][2].GetFloat()};
                vec3 color{comp->value["color"][0].GetFloat(), comp->value["color"][1].GetFloat(), comp->value["color"][2].GetFloat()};
                registry->add<Light>(id, ambStr, ambColor, lightStr, lightColor, color);
                factory->getShader<PhongShader>()->setLight(id);
            }
            else if (comp->name == "sound") {
                std::string filename{comp->value["filename"].GetString()};
                bool looping{comp->value["loop"].GetBool()};
                float gain{comp->value["gain"].GetFloat()};
                registry->add<Sound>(id, filename, looping, gain);
            }
            else if (comp->name == "particleemitter") {
                bool active{comp->value["isactive"].GetBool()};
                bool decay{comp->value["decay"].GetBool()};
                size_t numParticles{static_cast<size_t>(comp->value["numparticles"].GetInt())};
                int pps{comp->value["pps"].GetInt()};
                const vec3 initDir{comp->value["initdir"][0].GetFloat(), comp->value["initdir"][1].GetFloat(), comp->value["initdir"][2].GetFloat()};
                const QColor initColor{comp->value["initColor"][0].GetInt(), comp->value["initColor"][1].GetInt(),
                                       comp->value["initColor"][2].GetInt(), comp->value["initColor"][3].GetInt()};
                float speed{comp->value["speed"].GetFloat()};
                float size{comp->value["size"].GetFloat()};
                float spread{comp->value["spread"].GetFloat()};
                float lifespan{comp->value["lifespan"].GetFloat()};
                GLuint textureUnit{comp->value["textureunit"].GetUint()};
                registry->add<ParticleEmitter>(id, active, decay, numParticles, pps, initDir, initColor, speed, size, spread, lifespan, textureUnit);
            }
            else if (comp->name == "billboard") {
                bool constantYUp{comp->value["y-up"].GetBool()};
                bool normalVersion{comp->value["normalversion"].GetBool()};
                registry->add<BillBoard>(id, constantYUp, normalVersion);
            }
            else if (comp->name == "AABB") {
                vec3 origin{comp->value["origin"][0].GetFloat(), comp->value["origin"][1].GetFloat(), comp->value["origin"][2].GetFloat()};
                vec3 size{comp->value["size"][0].GetFloat(), comp->value["size"][1].GetFloat(), comp->value["size"][2].GetFloat()};
                bool isStatic{comp->value["static"].GetBool()};
                registry->add<AABB>(id, origin, size, isStatic);
            }
            else if (comp->name == "Sphere") {
                vec3 position{comp->value["position"][0].GetFloat(), comp->value["position"][1].GetFloat(), comp->value["position"][2].GetFloat()};
                float radius{comp->value["radius"].GetFloat()};
                bool isStatic{comp->value["static"].GetBool()};
                registry->add<Sphere>(id, position, radius, isStatic);
            }
            else if (comp->name == "AI") {
                int hp{comp->value["health"].GetInt()};
                float speed{comp->value["speed"].GetFloat()};
                registry->add<AIComponent>(id, hp, speed);
            }
            else if (comp->name == "Buildable") {
                bool buildable{comp->value["canBuild"].GetBool()};
                registry->add<Buildable>(id, buildable);
            }
            else if (comp->name == "BSplinePoint") {
                vec3 location{comp->value["location"][0].GetFloat(), comp->value["location"][1].GetFloat(), comp->value["location"][2].GetFloat()};
                registry->add<BSplinePoint>(id, location);
            }
            else if (comp->name == "GameCamera") {
                vec3 camPos{comp->value["position"][0].GetFloat(), comp->value["position"][1].GetFloat(), comp->value["position"][2].GetFloat()};
                float pitch{comp->value["pitch"].GetFloat()};
                float yaw{comp->value["yaw"].GetFloat()};
                bool active{comp->value["active"].GetBool()};
                registry->add<GameCamera>(id, camPos, pitch, yaw, active);
            }
        }
    }
    for (auto &pair : parentID) {
        // Pair key is the actual id of the child object. Pair value is the old id of the parent object.
        //We can find the new id of that object by searching idPairs using Pair value as the key.
        registry->get<Transform>(pair.first).parentID = idPairs[pair.second];
    }
}
void Scene::loadSceneFromFile(const QString &fileName)
{
    std::ifstream file{gsl::sceneFilePath + fileName.toStdString()};
    if (!file.good()) {
        qDebug() << "Can't read the JSON scene file!";
        return;
    }
    std::stringstream stream;
    stream << file.rdbuf();
    const std::string fileCopy{stream.str()};
    const char *scene{fileCopy.c_str()};
    Document sceneDoc;
    sceneDoc.Parse(scene);
    populateScene(sceneDoc);
    mScenes[fileName] = fileCopy;
}
QString Scene::name() const
{
    return mName;
}


