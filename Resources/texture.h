#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLFunctions_4_1_Core>

/**
    \brief Simple class for creating textures from an image file.
 */
class Texture : protected QOpenGLFunctions_4_1_Core {
private:
    GLuint mId{0};


public:
    /**
     * Read an image file and create a texture with standard parameters.
     * @param filename The name of the image file containing a texture.
     * @param textureUnit The size of the mTextures map in ResourceManager before this texture was added.
     */
    Texture(const std::string &filename, GLuint textureUnit = 0);
    /**
     * Read an image file and create a cubemap with standard parameters.
     * @param filename The name of the image file containing a texture.
     * @param textureUnit The size of the mTextures map in ResourceManager before this texture was added.
     */
    Texture(std::vector<std::string> faces, GLuint textureUnit = 0);
    /**
    * Return the id of a previously generated texture object.
    * @return The id of a previously generated texture object
    */
    GLuint id() const;
    bool isValid{true};

    /**
     * Get the texture unit for use with a Material component.
     * @return
     */
    GLuint textureUnit() const;
    GLuint mTextureUnit{0};
private:
    /**
     * Loads a texture from file.
     * @param directory
     * @return
     */
    bool textureFromFile(const std::string &directory /*, bool gamma = false*/);
    /**
     * Loads a cubemap from a collection of images.
     * @param faces
     * @return
     */
    bool cubeMapFromFile(std::vector<std::string> faces);
};

#endif // TEXTURE_H
