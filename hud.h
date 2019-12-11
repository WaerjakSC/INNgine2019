#ifndef HUD_H
#define HUD_H
#include "vector2d.h"


class hud {
public:
    hud();
    hud(int texture, gsl::Vector2D position, gsl::Vector2D scale);


    int getTexture() const;
    gsl::Vector2D getPosition() const;
    gsl::Vector2D getScale() const;

private:
    int texture;
    gsl::Vector2D position;
    gsl::Vector2D scale;
};

#endif // HUD_H
