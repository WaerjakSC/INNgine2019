#include "texture.h"
#include "innpch.h"

Texture::Texture(const std::string &filename, GLuint textureUnit)
    : QOpenGLFunctions_4_1_Core() {
    isValid = TextureFromFile(filename, textureUnit);
}

GLuint Texture::id() const {
    return mId;
}
bool Texture::TextureFromFile(const std::string &filename, GLuint textureUnit /*, bool gamma*/) {
    initializeOpenGLFunctions();
    std::string fileWithPath = gsl::assetFilePath + "Textures/" + filename;

    glGenTextures(1, &mId);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fileWithPath.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;
        // activate the texture unit first before binding texture
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, mId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        qDebug() << "Unable to read " << QString(fileWithPath.c_str());
        stbi_image_free(data);
        return false;
    }

    return true;
}
