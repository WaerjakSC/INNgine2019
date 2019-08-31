#include "resourcemanager.h"
#include <QDebug>


std::map<std::string, Shader*> ResourceManager::Shaders;

ResourceManager::ResourceManager()
{

}

void ResourceManager::Clear()
{

}


Shader ResourceManager::LoadShader(std::string name, Shader *shader)
{
    if(shader && Shaders.find(name) == Shaders.end()){
        Shaders[name] = shader;

        qDebug() << "ResourceManager: Added shader " << QString::fromStdString(name);
    }
}

Shader *ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture ResourceManager::LoadTexture(std::string name)
{

}

Texture *ResourceManager::GetTexture(std::string name){

}

meshData ResourceManager::ReadMesh(std::string name){

}

