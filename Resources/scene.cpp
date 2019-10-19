#include "scene.h"
#include "constants.h"
#include "registry.h"
#include "resourcemanager.h"
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

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

        writer.String("GameObject");
        writer.StartObject();
        writer.Key("name");
        writer.String(entity.second->name().toStdString().c_str());
        writer.Key("id");
        writer.Uint(entity.second->id());
        writer.Key("components");
        writer.StartObject();
        CType typeMask = entity.second->types();
        if ((typeMask & CType::Transform) != CType::None) {
            const Transform trans = registry->getComponent<Transform>(entity.second->id());
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
        if ((typeMask & CType::Material) != CType::None) {
            writer.Key("material");
            writer.StartObject();
            const Material mat = registry->getComponent<Material>(entity.second->id());

            writer.Key("color");
            writer.StartArray();
            writer.Double(mat.mObjectColor.x);
            writer.Double(mat.mObjectColor.y);
            writer.Double(mat.mObjectColor.z);
            writer.EndArray();

            writer.Key("textureid");
            writer.Int(mat.mTextureUnit);

            writer.Key("shader");
            switch (mat.mShader) {
            case Color:
                writer.String("color");
                break;
            case Tex:
                writer.String("texture");
                break;
            case Phong:
                writer.String("phong");
                break;
            }
            writer.EndObject();
        }
        if ((typeMask & CType::Mesh) != CType::None) {
            writer.Key("mesh");
            writer.StartObject();
            const Mesh mesh = registry->getComponent<Mesh>(entity.second->id());
            if (mesh.mName == "")
                qDebug() << "what happens here?";
            writer.Key("name");
            writer.String(mesh.mName.c_str()); // Use the mesh name (either a prefab or a file in Assets/Meshes) to find out what to do from here.
            writer.EndObject();
        }
        if ((typeMask & CType::Light) != CType::None) {
            writer.Key("light");
            writer.StartObject();
            const Light lightcomp = registry->getComponent<Light>(entity.second->id());
            LightData light = lightcomp.mLight;
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

            writer.Key("specstr");
            writer.Double(light.mSpecularStrength);
            writer.Key("specexp");
            writer.Int(light.mSpecularExponent);

            writer.Key("color");
            writer.StartArray();
            writer.Double(light.mObjectColor.x);
            writer.Double(light.mObjectColor.y);
            writer.Double(light.mObjectColor.z);
            writer.EndArray();

            writer.EndObject();
        }
        if ((typeMask & CType::Collision) != CType::None) {
            // Write Collision data to json here.
        }
        writer.EndObject();
        writer.EndObject();
    }
    writer.EndObject();
    std::ofstream of(gsl::sceneFilePath + fileName.toStdString());
    of << buf.GetString();
    if (!of.good() || !of)
        throw std::runtime_error("Can't write the JSON string to the file!");
}
void Scene::loadScene(const QString &fileName) {
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
        if (itr->value["name"] == "XYZ") {
            factory->makeXYZ();
        } else {
            GLuint id = registry->makeEntity(itr->value["name"].GetString());
            idPairs[itr->value["id"].GetInt()] = id;
            if (itr->value["name"] == "Light")
                controllerID = id;
            for (Value::ConstMemberIterator comp = itr->value["components"].MemberBegin(); comp != itr->value["components"].MemberEnd(); ++comp) {
                //                qDebug() << comp->name.GetString();
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
                } else if (comp->name == "material") {
                    gsl::Vector3D color(comp->value["color"][0].GetDouble(), comp->value["color"][1].GetDouble(), comp->value["color"][2].GetDouble());
                    registry->addComponent<Material>(id, comp->value["shader"].GetString(), comp->value["textureid"].GetInt(), color);
                } else if (comp->name == "mesh") {
                    if (comp->value["name"] == "BillBoard") {
                        factory->makeBillBoardMesh(id);
                        registry->addBillBoard(id);
                    } else if (comp->value["name"] == "Skybox")
                        factory->makeSkyBoxMesh(id);
                    else if (comp->value["name"] == "Light")
                        factory->makeLightMesh(id);
                    else
                        factory->addMeshComponent(comp->value["name"].GetString(), id);
                } else if (comp->name == "light") {
                    GLfloat ambStr = comp->value["ambstr"].GetFloat();
                    gsl::Vector3D ambColor(comp->value["ambcolor"][0].GetDouble(), comp->value["ambcolor"][1].GetDouble(), comp->value["ambcolor"][2].GetDouble());
                    GLfloat lightStr = comp->value["lightstr"].GetFloat();
                    gsl::Vector3D lightColor(comp->value["lightcolor"][0].GetDouble(), comp->value["lightcolor"][1].GetDouble(), comp->value["lightcolor"][2].GetDouble());
                    GLfloat specStr = comp->value["specstr"].GetFloat();
                    GLint specExp = comp->value["specexp"].GetInt();
                    gsl::Vector3D color(comp->value["color"][0].GetDouble(), comp->value["color"][1].GetDouble(), comp->value["color"][2].GetDouble());

                    LightData data(ambStr, ambColor, lightStr, lightColor, specStr, specExp, color);
                    registry->addComponent<Light>(id, data);
                } else if (comp->name == "collision") {
                    // Set collision variables here
                }
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
    if (!file.good())
        throw std::runtime_error("Can't read the JSON file!");
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
