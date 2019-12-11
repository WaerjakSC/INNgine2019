#include "hud.h"
#include "texture.h"

hud::hud() {

}
hud::hud(int texture, gsl::Vector2D position, gsl::Vector2D scale) {
    this->texture = texture;
    this->position = position;
    this->scale = scale;
}
int hud::getTexture() const {
    return texture;
}
gsl::Vector2D hud::getPosition() const {
    return position;
}
gsl::Vector2D hud::getScale() const {
    return scale;
}
