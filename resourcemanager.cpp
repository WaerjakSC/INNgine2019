#include "resourcemanager.h"
#include <QDebug>

std::map<std::string, Shader *> ResourceManager::Shaders;
std::map<std::string, Texture *> ResourceManager::Textures;
std::map<std::string, meshData *> ResourceManager::Meshes;

ResourceManager::ResourceManager() {
}

// Do resource manager stuff -- Aka actually delete the pointers after application end
ResourceManager::~ResourceManager() {
    for (auto &shader : Shaders) {
        delete shader.second;
    }
    for (auto &texture : Textures) {
        delete texture.second;
    }
}

void ResourceManager::LoadShader(std::string name, Shader *shader) {
    if (shader && Shaders.find(name) == Shaders.end()) {
        Shaders[name] = shader;

        qDebug() << "ResourceManager: Added shader " << QString::fromStdString(name);
    }
}

void ResourceManager::LoadTexture(std::string name) {
}

Shader *ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}
Texture *ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

meshData *ResourceManager::ReadMesh(std::string name) {
    return Meshes[name];
}
