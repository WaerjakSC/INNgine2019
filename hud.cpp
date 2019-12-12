#include "HUD.h"
#include "texture.h"
#include "ECS/registry.h"
#include "ECS/view.h"
#include "ECS/components.h"
#include "resourcemanager.h"

HUD::HUD() {

}
HUD::HUD(int texture, gsl::Vector2D position, gsl::Vector2D scale) {
    this->texture = texture;
    this->position = position;
    this->scale = scale;
}
gsl::Vector2D HUD::getPosition() const {
    return position;
}
gsl::Vector2D HUD::getScale() const {
    return scale;
}
std::string HUD::currentTexture(){
    Registry *registry = Registry::instance();
    int currenthealth = registry->getPlayer().health;
    if (currenthealth == 4){
        return four;
    }
    else if (currenthealth == 3){
        return three;
    }
    else if (currenthealth == 2){
        return two;
    }
    else if (currenthealth == 1){
        return one;
    }
    else if (currenthealth == 0){
        return zero;
    }
    else{
        return five;
    }
}

void HUD::updatehealth() {

    Registry *registry = Registry::instance();
    ResourceManager *resourcemanager = ResourceManager::instance();
    auto billboardView{ registry->view<BillBoard, Transform, Material>()};
    for (auto entity : billboardView) {
        auto [billboard, transform, mat]{billboardView.get<BillBoard, Transform, Material>(entity)};
        mat.textureUnit = resourcemanager->getTexture(currentTexture())->mTextureUnit;
    }
}
