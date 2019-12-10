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

    void saveScene(const QString &fileName);

    void loadScene(const QString &fileName);

    QString name() const;

    GLuint mLight;
    GLuint mGameCamID;

private:
    std::map<QString, std::string> mScenes;
    QString mName;
    void loadSceneFromFile(const QString &fileName);
    void populateScene(const Document &scene);
};

#endif // SCENE_H
