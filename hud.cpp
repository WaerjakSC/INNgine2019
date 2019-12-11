#include "HUD.h"
#include "texture.h"

HUD::HUD() {

}
HUD::HUD(int texture, gsl::Vector2D position, gsl::Vector2D scale) {
    this->texture = texture;
    this->position = position;
    this->scale = scale;
}
int HUD::getTexture() const {
    return texture;
}
gsl::Vector2D HUD::getPosition() const {
    return position;
}
gsl::Vector2D HUD::getScale() const {
    return scale;
}
