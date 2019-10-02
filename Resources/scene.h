#ifndef SCENE_H
#define SCENE_H
#include "gsl_math.h"
#include "rapidjson/document.h"

using namespace rapidjson;

class Scene {

public:
    Scene();

    void saveScene(std::string fileName);

    void loadScene(std::string fileName);

    GLuint controllerID;

private:
    Document mScene;
    std::string mName{"Basic Scene"};
};

#endif // SCENE_H
