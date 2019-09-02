#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Components/meshcomponent.h"
#include "Shaders/shader.h"
#include "texture.h"
#include <map>
#include <string>

// Resource manager work in progress
class ResourceManager {
public:
    // Static std::map(key, object) for resource storage
    static std::map<std::string, Shader *> Shaders;
    static std::map<std::string, Texture *> Textures;
    static std::map<std::string, meshData> Meshes;
    // Loads and generates shader (takes name and the shader)
    static void LoadShader(std::string name, Shader *shader);
    // Gets stored shader
    static Shader *GetShader(std::string name);
    // Loads and generates texture from file
    static void LoadTexture(std::string name);
    // Gets stored texture
    //    static Texture *GetTexture(std::string name);
    //    // Reads and loads mesh
    //    static meshData ReadMesh(std::string name);
    // De-allocate loaded resources from memory
    static void Clear();

    virtual ~ResourceManager();

private:
    // Private constructor
    ResourceManager();
};

#endif // RESOURCEMANAGER_H
