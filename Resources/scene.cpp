#include "scene.h"
#include "constants.h"
#include "inputsystem.h"
#include "movementsystem.h"
#include "phongshader.h"
#include "registry.h"
#include "resourcemanager.h"
#include "shader.h"
#include "textureshader.h"
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <sstream>

Scene::Scene() {
}

void Scene::saveScene(const QString &fileName) {
    if (fileName.isEmpty())
        return;

    Registry *registry = Registry::instance();
    std::map<GLuint, Entity *> entities = registry->getEntities();

    StringBuffer buf;
    PrettyWriter<StringBuffer> writer(buf);

    mName = fileName.chopped(5);
    writer.StartObject();

    for (auto entity : entities) {
        GLuint eID = entity.first;
        if (eID != 0 && !entity.second->isEmpty()) { // Ignore the first entity, it's reserved for the XYZ lines. (Hardcoded in RenderWindow to be loaded before loadProject, so it's always first)
            writer.String("GameObject");
            writer.StartObject();
            writer.Key("name");
            writer.String(entity.second->name().toStdString().c_str());
            writer.Key("id");
            writer.Uint(eID);
            writer.Key("components");
            writer.StartObject();
            if (registry->contains<Transform>(eID)) {
                const Transform trans = registry->getComponent<Transform>(eID);
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
                const Material mat = registry->getComponent<Material>(eID);

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

                writer.Key("textureid");
                writer.Int(mat.mTextureUnit);

                writer.Key("shader");
                writer.String(mat.mShader->getName().c_str());

                writer.EndObject();
            }
            if (registry->contains<Mesh>(eID)) {
                writer.Key("mesh");
                writer.StartObject();
                const Mesh mesh = registry->getComponent<Mesh>(eID);
                if (mesh.mName == "")
                    qDebug() << "No mesh name!";
                writer.Key("name");
                writer.String(mesh.mName.c_str()); // Use the mesh name (either a prefab or a file in Assets/Meshes) to find out what to do from here.
                writer.EndObject();
            }
            if (registry->contains<Light>(eID)) {
                writer.Key("light");
                writer.StartObject();
                const Light &light = registry->getComponent<Light>(eID);

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
            if (registry->contains<Input>(eID)) {
                writer.Key("input");
                writer.StartObject();
                writer.Key("isController");
                writer.Bool(true);
                writer.EndObject();
            }
            if (registry->contains<Sound>(eID)) {
                const Sound &sound = registry->getComponent<Sound>(eID);
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
            if (registry->contains<Collision>(eID)) {
                // Write Collision data to json here.
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
    // Iterate through each gameobject in the scene
    for (Value::ConstMemberIterator itr = scene.MemberBegin(); itr != scene.MemberEnd(); ++itr) {
        // Iterate through each of the members in the gameobject (name, id, components)
        GLuint id = registry->makeEntity(itr->value["name"].GetString());
        idPairs[itr->value["id"].GetInt()] = id;
        for (Value::ConstMemberIterator comp = itr->value["components"].MemberBegin(); comp != itr->value["components"].MemberEnd(); ++comp) {
            if (comp->name == "transform") {
                gsl::Vector3D position(comp->value["position"][0].GetDouble(), comp->value["position"][1].GetDouble(), comp->value["position"][2].GetDouble());
                gsl::Vector3D rotation(comp->value["rotation"][0].GetDouble(), comp->value["rotation"][1].GetDouble(), comp->value["rotation"][2].GetDouble());
                gsl::Vector3D scale(comp->value["scale"][0].GetDouble(), comp->value["scale"][1].GetDouble(), comp->value["scale"][2].GetDouble());
                registry->addComponent<Transform>(id, position, rotation, scale);
                if (comp->value["parent"].GetInt() != -1) {
                    if (idPairs.find(comp->value["parent"].GetInt()) != idPairs.end())                            // if the parent has been parsed already
                        registry->getComponent<Transform>(id).parentID = idPairs[comp->value["parent"].GetInt()]; // use the old parentID to set the new parentID
                    else
                        parentID[id] = comp->value["parent"].GetInt(); // if not, add it to the list of parent/child pairs that must be updated after the loop
                }
                registry->getSystem<MovementSystem>()->updateEntity(id); // Note: Not sure about this line, but for now it should ensure that all transforms are correct as soon as they are created
            } else if (comp->name == "material") {
                gsl::Vector3D color(comp->value["color"][0].GetDouble(), comp->value["color"][1].GetDouble(), comp->value["color"][2].GetDouble());
                GLfloat specStr = comp->value["specstr"].GetFloat();
                GLint specExp = comp->value["specexp"].GetInt();
                QString shaderName = comp->value["shader"].GetString();
                Shader *shader{nullptr};
                ResourceManager *factory = ResourceManager::instance();
                if (shaderName == "PlainShader")
                    shader = factory->getShader<ColorShader>();
                else if (shaderName == "TextureShader")
                    shader = factory->getShader<TextureShader>();
                else if (shaderName == "PhongShader")
                    shader = factory->getShader<PhongShader>();
                registry->addComponent<Material>(id, shader, comp->value["textureid"].GetInt(), color, specStr, specExp);
            } else if (comp->name == "mesh") {
                factory->addMeshComponent(comp->value["name"].GetString(), id);
            } else if (comp->name == "light") { // Again, temporary, very static functionality atm
                GLfloat ambStr = comp->value["ambstr"].GetFloat();
                gsl::Vector3D ambColor(comp->value["ambcolor"][0].GetDouble(), comp->value["ambcolor"][1].GetDouble(), comp->value["ambcolor"][2].GetDouble());
                GLfloat lightStr = comp->value["lightstr"].GetFloat();
                gsl::Vector3D lightColor(comp->value["lightcolor"][0].GetDouble(), comp->value["lightcolor"][1].GetDouble(), comp->value["lightcolor"][2].GetDouble());
                gsl::Vector3D color(comp->value["color"][0].GetDouble(), comp->value["color"][1].GetDouble(), comp->value["color"][2].GetDouble());
                registry->addComponent<Light>(id, ambStr, ambColor, lightStr, lightColor, color);
                mLight = id;
            } else if (comp->name == "input") { // Atm just going to assume we only ever have one inputcomponent
                registry->addComponent<Input>(id);
                registry->getSystem<InputSystem>()->setPlayerController(id);
            } else if (comp->name == "sound") {
                std::string filename = comp->value["filename"].GetString();
                bool looping = comp->value["loop"].GetBool();
                float gain = comp->value["gain"].GetDouble();
                registry->addComponent<Sound>(id, filename, looping, gain);
            } else if (comp->name == "collision") {
                // Set collision variables here
            }
        }
    }
    for (auto &pair : parentID) {
        // Pair key is the actual id of the child object. Pair value is the old id of the parent object.
        //We can find the new id of that object by searching idPairs using Pair value as the key.
        registry->getComponent<Transform>(pair.first).parentID = idPairs[pair.second];
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
