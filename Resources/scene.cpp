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

Scene::Scene() {
}

void Scene::saveScene(const QString &fileName) {
    if (fileName.isEmpty())
        return;

    Registry *registry = Registry::instance();
    std::map<GLuint, cjk::Ref<Entity>> entities = registry->getEntities();
    auto inputSys = registry->getSystem<InputSystem>();

    StringBuffer buf;
    PrettyWriter<StringBuffer> writer(buf);

    mName = fileName;
    writer.StartObject();

    writer.String("controller");
    writer.StartObject();
    writer.Key("id");
    writer.Uint(inputSys->player());
    writer.EndObject();

    for (auto entity : entities) {
        GLuint eID = entity.first;
        if (eID != 0 && !entity.second->name().isEmpty()) { // Ignore the first entity, it's reserved for the XYZ lines. (Hardcoded in RenderWindow to be loaded before loadProject, so it's always first)
            writer.String("Entity");
            writer.StartObject();
            writer.Key("name");
            writer.String(entity.second->name().toStdString().c_str());
            writer.Key("id");
            writer.Uint(eID);
            writer.Key("components");
            writer.StartObject();
            if (registry->contains<Transform>(eID)) {
                const Transform trans = registry->get<Transform>(eID);
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
            if (registry->contains<Material>(eID)) {
                writer.Key("material");
                writer.StartObject();
                const Material &mat = registry->get<Material>(eID);

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
            if (registry->contains<Mesh>(eID)) {
                writer.Key("mesh");
                writer.StartObject();
                const Mesh &mesh = registry->get<Mesh>(eID);
                //                if (mesh.mName == "")
                //                    qDebug() << "No mesh name!";
                writer.Key("name");
                writer.String(mesh.mName.c_str()); // Use the mesh name (either a prefab or a file in Assets/Meshes) to find out what to do from here.
                writer.EndObject();
            }
            if (registry->contains<Light>(eID)) {
                writer.Key("light");
                writer.StartObject();
                const Light &light = registry->get<Light>(eID);

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
            if (registry->contains<Sound>(eID)) {
                const Sound &sound = registry->get<Sound>(eID);
                writer.Key("sound");
                writer.StartObject();
                writer.Key("filename");
                writer.String(sound.mName.c_str());
                writer.Key("loop");
                writer.Bool(sound.mLooping);
                writer.Key("gain");
                writer.Double(sound.mGain);
                writer.EndObject();
            }
            if (registry->contains<AABB>(eID)) {
                writer.Key("AABB");
                writer.StartObject();
                const AABB &aabb = registry->get<AABB>(eID);

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
            if (registry->contains<OBB>(eID)) {
                writer.Key("OBB");
                writer.StartObject();
                const OBB &obb = registry->get<OBB>(eID);

                writer.Key("position");
                writer.StartArray();
                writer.Double(obb.position.x);
                writer.Double(obb.position.y);
                writer.Double(obb.position.z);
                writer.EndArray();
                writer.Key("size");
                writer.StartArray();
                writer.Double(obb.size.x);
                writer.Double(obb.size.y);
                writer.Double(obb.size.z);
                writer.EndArray();
                writer.Key("static");
                writer.Bool(obb.isStatic);
                // Need to do something with the rotation mat3
                writer.EndObject();
            }
            if (registry->contains<Plane>(eID)) {
                writer.Key("Plane");
                writer.StartObject();
                const Plane &plane = registry->get<Plane>(eID);

                writer.Key("normal");
                writer.StartArray();
                writer.Double(plane.normal.x);
                writer.Double(plane.normal.y);
                writer.Double(plane.normal.z);
                writer.EndArray();
                writer.Key("distance");
                writer.Double(plane.distance);
                writer.Key("static");
                writer.Bool(plane.isStatic);
                writer.EndObject();
            }
            if (registry->contains<Sphere>(eID)) {
                writer.Key("Sphere");
                writer.StartObject();
                const Sphere &sphere = registry->get<Sphere>(eID);

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
            if (registry->contains<Cylinder>(eID)) {
                writer.Key("Cylinder");
                writer.StartObject();
                const Cylinder &cylinder = registry->get<Cylinder>(eID);

                writer.Key("position");
                writer.StartArray();
                writer.Double(cylinder.position.x);
                writer.Double(cylinder.position.y);
                writer.Double(cylinder.position.z);
                writer.EndArray();
                writer.Key("radius");
                writer.Double(cylinder.radius);
                writer.Key("height");
                writer.Double(cylinder.height);
                writer.Key("static");
                writer.Bool(cylinder.isStatic);
                writer.EndObject();
            }
            if (registry->contains<AIComponent>(eID)) {
                writer.Key("AI");
                writer.StartObject();
                const AIComponent &ai = registry->get<AIComponent>(eID);

                writer.Key("health");
                writer.Int(ai.hp);
                writer.Key("damage");
                writer.Int(ai.damage);
                writer.EndObject();
            }
            if (registry->contains<BSplinePoint>(eID)) {
                writer.Key("BSplinePoint");
                writer.StartObject();
                const BSplinePoint &cp = registry->get<BSplinePoint>(eID);
                writer.Key("location");
                writer.StartArray();
                writer.Double(cp.location.x);
                writer.Double(cp.location.y);
                writer.Double(cp.location.z);
                writer.EndArray();
                writer.EndObject();
            }
            if (registry->contains<GameCamera>(eID)) {
                writer.Key("GameCamera");
                writer.StartObject();
                const GameCamera &cam = registry->get<GameCamera>(eID);
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
    std::ofstream of(gsl::sceneFilePath + fileName.toStdString() + ".json");
    of << buf.GetString();
    if (!of.good() || !of)
        throw std::runtime_error("Can't write the JSON string to the file!");
}
void Scene::loadScene(const QString &fileName) {
    if (fileName.isEmpty()) {
        return;
    }
    Registry *registry = Registry::instance();
    ResourceManager *factory = ResourceManager::instance();
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
void Scene::populateScene(const Document &scene) {
    std::map<int, int> parentID;
    std::map<int, int> idPairs;
    Registry *registry = Registry::instance();
    ResourceManager *factory = ResourceManager::instance();
    //        Ref<GameCameraController> gameCam = std::make_shared<GameCameraController>();
    //        registry->getSystem<InputSystem>()->setGameCameraController(gameCam, mGameCamID);

    if (scene.HasMember("controller")) {
        GLuint controllerID = scene["controller"]["id"].GetUint();
        registry->getSystem<InputSystem>()->setPlayer(controllerID);
    } else
        registry->getSystem<InputSystem>()->setPlayer(0);
    if (!scene.HasMember("Entity"))
        return;
    // Iterate through each entity in the scene
    for (Value::ConstMemberIterator itr = scene.MemberBegin(); itr != scene.MemberEnd(); ++itr) {
        // Iterate through each of the members in the entity (name, id, components)
        if (itr->name == "controller") // Note: Naming an entity "controller" probably fucks with this
            continue;
        GLuint id = registry->makeEntity(itr->value["name"].GetString());
        idPairs[itr->value["id"].GetInt()] = id;
        for (Value::ConstMemberIterator comp = itr->value["components"].MemberBegin(); comp != itr->value["components"].MemberEnd(); ++comp) {
            if (comp->name == "transform") {
                vec3 position(comp->value["position"][0].GetDouble(), comp->value["position"][1].GetDouble(), comp->value["position"][2].GetDouble());
                vec3 rotation(comp->value["rotation"][0].GetDouble(), comp->value["rotation"][1].GetDouble(), comp->value["rotation"][2].GetDouble());
                vec3 scale(comp->value["scale"][0].GetDouble(), comp->value["scale"][1].GetDouble(), comp->value["scale"][2].GetDouble());
                registry->add<Transform>(id, position, rotation, scale);
                if (comp->value["parent"].GetInt() != -1) {
                    if (idPairs.find(comp->value["parent"].GetInt()) != idPairs.end())                   // if the parent has been parsed already
                        registry->get<Transform>(id).parentID = idPairs[comp->value["parent"].GetInt()]; // use the old parentID to set the new parentID
                    else
                        parentID[id] = comp->value["parent"].GetInt(); // if not, add it to the list of parent/child pairs that must be updated after the loop
                }
                registry->getSystem<MovementSystem>()->updateEntity(id); // Note: Not sure about this line, but for now it should ensure that all transforms are correct as soon as they are created
            } else if (comp->name == "material") {
                vec3 color(comp->value["color"][0].GetDouble(), comp->value["color"][1].GetDouble(), comp->value["color"][2].GetDouble());
                GLfloat specStr = comp->value["specstr"].GetFloat();
                GLint specExp = comp->value["specexp"].GetInt();
                QString shaderName = comp->value["shader"].GetString();
                cjk::Ref<Shader> shader{nullptr};
                ResourceManager *factory = ResourceManager::instance();
                if (shaderName == "PlainShader")
                    shader = factory->getShader<ColorShader>();
                else if (shaderName == "TextureShader")
                    shader = factory->getShader<TextureShader>();
                else if (shaderName == "PhongShader")
                    shader = factory->getShader<PhongShader>();
                else if (shaderName == "SkyboxShader")
                    shader = factory->getShader<SkyboxShader>();
                registry->add<Material>(id, shader, comp->value["textureunit"].GetInt(), color, specStr, specExp);
            } else if (comp->name == "mesh") {
                std::string meshName = comp->value["name"].GetString();
                factory->addMeshComponent(meshName, id);
                if (meshName == "Skybox")
                    registry->getSystem<RenderSystem>()->setSkyBoxID(id);
            } else if (comp->name == "light") { // Again, temporary, very static functionality atm
                GLfloat ambStr = comp->value["ambstr"].GetFloat();
                vec3 ambColor(comp->value["ambcolor"][0].GetDouble(), comp->value["ambcolor"][1].GetDouble(), comp->value["ambcolor"][2].GetDouble());
                GLfloat lightStr = comp->value["lightstr"].GetFloat();
                vec3 lightColor(comp->value["lightcolor"][0].GetDouble(), comp->value["lightcolor"][1].GetDouble(), comp->value["lightcolor"][2].GetDouble());
                vec3 color(comp->value["color"][0].GetDouble(), comp->value["color"][1].GetDouble(), comp->value["color"][2].GetDouble());
                registry->add<Light>(id, ambStr, ambColor, lightStr, lightColor, color);
                mLight = id;
                factory->getShader<PhongShader>()->setLight(registry->getEntity(mLight));
            } else if (comp->name == "sound") {
                std::string filename = comp->value["filename"].GetString();
                bool looping = comp->value["loop"].GetBool();
                float gain = comp->value["gain"].GetDouble();
                registry->add<Sound>(id, filename, looping, gain);
            } else if (comp->name == "AABB") {
                vec3 origin(comp->value["origin"][0].GetDouble(), comp->value["origin"][1].GetDouble(), comp->value["origin"][2].GetDouble());
                vec3 size(comp->value["size"][0].GetDouble(), comp->value["size"][1].GetDouble(), comp->value["size"][2].GetDouble());
                bool isStatic = comp->value["static"].GetBool();
                registry->add<AABB>(id, origin, size, isStatic);
            } else if (comp->name == "OBB") {
                vec3 position(comp->value["position"][0].GetDouble(), comp->value["position"][1].GetDouble(), comp->value["position"][2].GetDouble());
                vec3 size(comp->value["size"][0].GetDouble(), comp->value["size"][1].GetDouble(), comp->value["size"][2].GetDouble());
                // Need rotation matrix here
                bool isStatic = comp->value["static"].GetBool();
                registry->add<OBB>(id, position, size, isStatic);
            } else if (comp->name == "Plane") {
                vec3 normal(comp->value["normal"][0].GetDouble(), comp->value["normal"][1].GetDouble(), comp->value["origin"][2].GetDouble());
                float size = comp->value["distance"].GetDouble();
                // Need rotation matrix here
                bool isStatic = comp->value["static"].GetBool();
                registry->add<Plane>(id, normal, size, isStatic);
            } else if (comp->name == "Sphere") {
                vec3 position(comp->value["position"][0].GetDouble(), comp->value["position"][1].GetDouble(), comp->value["position"][2].GetDouble());
                float radius = comp->value["radius"].GetDouble();
                // Need rotation matrix here
                bool isStatic = comp->value["static"].GetBool();
                registry->add<Sphere>(id, position, radius, isStatic);
            } else if (comp->name == "Cylinder") {
                vec3 position(comp->value["position"][0].GetDouble(), comp->value["position"][1].GetDouble(), comp->value["position"][2].GetDouble());
                float radius = comp->value["radius"].GetDouble();
                float height = comp->value["height"].GetDouble();
                // Need rotation matrix here
                bool isStatic = comp->value["static"].GetBool();
                registry->add<Cylinder>(id, position, radius, height, isStatic);
            } else if (comp->name == "AI") {
                int hp = comp->value["health"].GetInt();
                int damage = comp->value["damage"].GetInt();
                registry->add<AIComponent>(id, hp, damage);
            } else if (comp->name == "BSplinePoint") {
                vec3 location(comp->value["location"][0].GetDouble(), comp->value["location"][1].GetDouble(), comp->value["location"][2].GetDouble());
                registry->add<BSplinePoint>(id, location);
            } else if (comp->name == "GameCamera") {
                vec3 camPos(comp->value["position"][0].GetDouble(), comp->value["position"][1].GetDouble(), comp->value["position"][2].GetDouble());
                float pitch = comp->value["pitch"].GetDouble();
                float yaw = comp->value["yaw"].GetDouble();
                bool active = comp->value["active"].GetBool();
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
void Scene::loadSceneFromFile(const QString &fileName) {
    std::ifstream file(gsl::sceneFilePath + fileName.toStdString());
    if (!file.good()) {
        qDebug() << "Can't read the JSON scene file!";
        return;
    }
    std::stringstream stream;
    stream << file.rdbuf();
    const std::string fileCopy = stream.str();
    const char *scene = fileCopy.c_str();
    Document sceneDoc;
    sceneDoc.Parse(scene);
    populateScene(sceneDoc);
    mScenes[fileName] = fileCopy;
}
QString Scene::name() const {
    return mName;
}
