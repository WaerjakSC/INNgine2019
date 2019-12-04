#include "texture.h"
#include "innpch.h"
namespace cjk {
Texture::Texture(const std::string &filename, GLuint textureUnit) : QOpenGLFunctions_4_1_Core{}, mTextureUnit{textureUnit}
{
    isValid = textureFromFile(filename);
}

Texture::Texture(std::vector<std::string> faces, GLuint textureUnit) : QOpenGLFunctions_4_1_Core{}, mTextureUnit{textureUnit}
{
    isValid = cubeMapFromFile(faces);
}

GLuint Texture::id() const
{
    return mId;
}
GLuint Texture::textureUnit() const
{
    return mTextureUnit;
}
bool Texture::cubeMapFromFile(std::vector<std::string> faces)
{
    initializeOpenGLFunctions();

    glGenTextures(1, &mId);
    // activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0 + mTextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mId);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i{0}; i < faces.size(); i++) {
        std::string fileWithPath{gsl::assetFilePath + "Textures/" + faces[i]};
        unsigned char *data{stbi_load(fileWithPath.c_str(), &width, &height, &nrChannels, 0)};
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            qDebug() << "Cubemap texture failed to load at path: " << QString::fromStdString(faces[i]);
            stbi_image_free(data);
            return false;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return true;
}
bool Texture::textureFromFile(const std::string &filename /*, bool gamma*/)
{
    initializeOpenGLFunctions();
    std::string fileWithPath{gsl::assetFilePath + "Textures/" + filename};
    glGenTextures(1, &mId);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data{stbi_load(fileWithPath.c_str(), &width, &height, &nrComponents, 0)};
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;
        // activate the texture unit first before binding texture
        glActiveTexture(GL_TEXTURE0 + mTextureUnit);
        glBindTexture(GL_TEXTURE_2D, mId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        qDebug() << "Unable to read " << QString(fileWithPath.c_str());
        stbi_image_free(data);
        return false;
    }

    return true;
}

} // namespace cjk
