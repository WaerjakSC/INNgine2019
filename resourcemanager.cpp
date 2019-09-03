#include "resourcemanager.h"
#include "Shaders/colorshader.h"
#include "Shaders/phongshader.h"
#include "Shaders/textureshader.h"
#include "innpch.h"
#include <QDebug>

std::map<ShaderType, Shader *> ResourceManager::Shaders;
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
    for (auto &mesh : Meshes) {
        delete mesh.second;
    }
}

void ResourceManager::LoadShader(ShaderType type, const GLchar *geometryPath) {
    if (Shaders.find(type) == Shaders.end()) {
        std::string shaderName;
        switch (type) {
        case ShaderType::Color:
            shaderName = "plainshader";
            Shaders[type] = new ColorShader(shaderName, geometryPath);
            break;
        case ShaderType::Tex:
            shaderName = "textureshader";
            Shaders[type] = new TextureShader(shaderName, geometryPath);
            break;
        case ShaderType::Phong:
            shaderName = "phongshader";
            Shaders[type] = new PhongShader(shaderName, geometryPath);
            break;
        default:
            qDebug() << "Failed to find shader in switch statement";
        }
        qDebug() << "ResourceManager: Added shader " << QString::fromStdString(shaderName);
    } else {
        qDebug() << "ResourceManager: Shader already loaded, ignoring...";
    }
}

void ResourceManager::LoadTexture(std::string fileName, GLuint textureUnit) {
    if (Textures.find(fileName) == Textures.end()) {
        Textures[fileName] = new Texture(fileName, textureUnit);

        qDebug() << "ResourceManager: Added texture" << QString::fromStdString(fileName);
    }
}
Shader *ResourceManager::GetShader(ShaderType type) {
    return Shaders[type];
}
Texture *ResourceManager::GetTexture(std::string fileName) {
    return Textures[fileName];
}
/**
 * @brief ResourceManager::LoadMesh - Loads the meshData from file if it isn't already in the Meshes map.
 * @param fileName
 * @return
 */
meshData *ResourceManager::LoadMesh(std::string fileName) {
    if (Meshes.find(fileName) == Meshes.end()) { // If the mesh isn't already in the Meshes array, create it.
        if (!readFile(fileName)) {               // Should run readFile and add the mesh to the Meshes map if it can be found
            qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
            return 0;
        }
        return Meshes[fileName];
    }
    return Meshes[fileName]; // Simply return the correct meshData if it has already been loaded
}
/**
 * @brief ResourceManager::GetMesh - Use this function if you know you've already loaded the mesh.
 * @param name
 * @return
 */
meshData *ResourceManager::GetMesh(std::string name) {
    return Meshes[name];
}
bool ResourceManager::readFile(std::string fileName) {
    auto *mesh = new meshData(); // Prepare the mesh data container
    //Open File
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + fileName;
    std::ifstream fileIn;
    fileIn.open(fileWithPath, std::ifstream::in);
    if (!fileIn) {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }

    //One line at a time-variable
    std::string oneLine;
    //One word at a time-variable
    std::string oneWord;

    std::vector<gsl::Vector3D> tempVertecies;
    std::vector<gsl::Vector3D> tempNormals;
    std::vector<gsl::Vector2D> tempUVs;

    // Variable for constructing the indices vector
    unsigned int temp_index = 0;

    //Reading one line at a time from file to oneLine
    while (std::getline(fileIn, oneLine)) {
        //Doing a trick to get one word at a time
        std::stringstream sStream;
        //Pushing line into stream
        sStream << oneLine;
        //Streaming one word out of line
        oneWord = ""; //resetting the value or else the last value might survive!
        sStream >> oneWord;

        if (oneWord == "#") {
            //Ignore this line
            //            qDebug() << "Line is comment "  << QString::fromStdString(oneWord);
            continue;
        }
        if (oneWord == "") {
            //Ignore this line
            //            qDebug() << "Line is blank ";
            continue;
        }
        if (oneWord == "v") {
            //            qDebug() << "Line is vertex "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector3D tempVertex;
            sStream >> oneWord;
            tempVertex.x = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.y = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempVertecies.push_back(tempVertex);

            continue;
        }
        if (oneWord == "vt") {
            //            qDebug() << "Line is UV-coordinate "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector2D tempUV;
            sStream >> oneWord;
            tempUV.x = std::stof(oneWord);
            sStream >> oneWord;
            tempUV.y = std::stof(oneWord);

            //UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            continue;
        }
        if (oneWord == "vn") {
            //            qDebug() << "Line is normal "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector3D tempNormal;
            sStream >> oneWord;
            tempNormal.x = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.y = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempNormals.push_back(tempNormal);
            continue;
        }
        if (oneWord == "f") {
            //            qDebug() << "Line is a face "  << QString::fromStdString(oneWord) << " ";
            //int slash; //used to get the / from the v/t/n - format
            int index, normal, uv;
            for (int i = 0; i < 3; i++) {
                sStream >> oneWord;                          //one word read
                std::stringstream tempWord(oneWord);         //to use getline on this one word
                std::string segment;                         //the numbers in the f-line
                std::vector<std::string> segmentArray;       //temp array of the numbers
                while (std::getline(tempWord, segment, '/')) //splitting word in segments
                {
                    segmentArray.push_back(segment);
                }
                index = std::stoi(segmentArray[0]); //first is vertex
                if (segmentArray[1] != "")          //second is uv
                    uv = std::stoi(segmentArray[1]);
                else {
                    //qDebug() << "No uvs in mesh";       //uv not present
                    uv = 0; //this will become -1 in a couple of lines
                }
                normal = std::stoi(segmentArray[2]); //third is normal

                //Fixing the indexes
                //because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1) //uv present!
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], tempUVs[uv]);
                    mesh->mVertices.push_back(tempVert);
                } else //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], gsl::Vector2D(0.0f, 0.0f));
                    mesh->mVertices.push_back(tempVert);
                }
                mesh->mIndices.push_back(temp_index++);
            }

            //For some reason the winding order is backwards so fixing this by swapping the last two indices
            //Update: this was because the matrix library was wrong - now it is corrected so this is no longer needed.
            //            unsigned int back = mIndices.size() - 1;
            //            std::swap(mIndices.at(back), mIndices.at(back-1));
            continue;
        }
    }

    //being a nice boy and closing the file after use
    fileIn.close();
    Meshes[fileName] = mesh; // Place the newly created mesh in the mesh map
    qDebug() << "Obj file read: " << QString::fromStdString(fileName);
    return true;
}

