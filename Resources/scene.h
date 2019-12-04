#ifndef SCENE_H
#define SCENE_H
#include "gsl_math.h"
#include "rapidjson/document.h"
#include <memory>
namespace cjk {
using namespace rapidjson;
class ResourceManager;
class Registry;
class Scene {
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
} // namespace cjk

#endif // SCENE_H
