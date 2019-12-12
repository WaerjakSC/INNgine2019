#ifndef SCENE_H
#define SCENE_H
#include "gltypes.h"
#include "rapidjson/document.h"
#include <QString>
#include <map>
#include <memory>

namespace gsl {
class Vector3D;
}

using namespace rapidjson;
class ResourceManager;
class Registry;
class Scene {
    using vec3 = gsl::Vector3D;

public:
    Scene() = default;

    /**
     * Save a scene and all of the entities and components in it to file.
     * @param fileName
     */
    void saveScene(const QString &fileName);

    /**
     * Parses and loads a scene from .json to the engine.
     * @param fileName
     */
    void loadScene(const QString &fileName);

    /**
     * The name of the current scene.
     * @return
     */
    QString name() const;

    GLuint mLight;
    GLuint mGameCamID;

private:
    std::map<QString, std::string> mScenes;
    QString mName;
    /**
     * Does the actual loading of the scene.
     * @param fileName
     */
    void loadSceneFromFile(const QString &fileName);

    /**
     * Fills the registry with the entities and components in the .json file.
     * @param scene
     */
    void populateScene(const Document &scene);
};

#endif // SCENE_H
