#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>

#include "texture.h"
#include "shader.h"
//#include "visualobject.h"
#include "objmesh.h"

// Resource manager work in progress

class ResourceManager
{
public:
    // Static std::map(key, object) for resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture> Textures;
    static std::map<std::string, ObjMesh> Meshes;
    // Loads and generates shader (takes name and the shader)
    static Shader LoadShader(std::string name, Shader *shader);
    // Gets stored shader
    Shader* GetShader(std::string name);
    // Loads and generates texture from file
    static Texture LoadTexture(std::string name);
    // Gets stored texture
    Texture* GetTexture(std::string name);
    // Reads and loads mesh
    static ObjMesh ReadMesh(std::string name);
    // De-allocate loaded resources from memory
    static void Clear();

private:
    // Private constructor
    ResourceManager();

};

#endif // RESOURCEMANAGER_H
