#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLFunctions_4_1_Core>

/**
    \brief Simple class for creating textures from an image file.
 */
class Texture : protected QOpenGLFunctions_4_1_Core {
private:
    GLuint mId{0};
    GLuint mTextureUnit{0};

public:
    /**
     * @brief Texture Read an image file and create a texture with standard parameters.
     * @param filename The name of the image file containing a texture.
     * @param textureUnit The size of the mTextures map in ResourceManager before this texture was added.
     */
    Texture(const std::string &filename, GLuint textureUnit = 0);
    /**
    * @brief id() Return the id of a previously generated texture object
    * @return The id of a previously generated texture object
    */
    GLuint id() const;
    bool isValid{true};

    GLuint textureUnit() const;

private:
    bool TextureFromFile(const std::string &directory /*, bool gamma = false*/);
};

#endif // TEXTURE_H
