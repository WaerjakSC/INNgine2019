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
std::string HUD::currentTexture(int currenthealth){
    if (currenthealth = 4){
        return four;
    }
    else if (currenthealth = 3){
        return three;
    }
    else if (currenthealth = 2){
        return two;
    }
    else if (currenthealth = 1){
        return one;
    }
    else if (currenthealth = 0){
        return zero;
    }
    else{
        return five;
    }
}
