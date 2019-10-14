#ifndef SCENE_H
#define SCENE_H
#include "gsl_math.h"
#include "rapidjson/document.h"
#include <memory>
using namespace rapidjson;
class ResourceManager;
class Registry;
class Scene {
public:
    Scene();

    void saveScene(const QString &fileName);

    void loadScene(const QString &fileName);

    GLuint controllerID;

    QString name() const;

private:
    std::map<QString, std::string> mScenes;
    QString mName;
    void loadSceneFromFile(const QString &fileName);
    void populateScene(const Document &scene);
};

#endif // SCENE_H
