#ifndef HUD_H
#define HUD_H
#include "vector2d.h"


class HUD {
public:
    HUD();
    HUD(int texture, gsl::Vector2D position, gsl::Vector2D scale);


    int getTexture() const;
    gsl::Vector2D getPosition() const;
    gsl::Vector2D getScale() const;
    std::string currentTexture(int currenthealth);

private:
    int texture;
    gsl::Vector2D position;
    gsl::Vector2D scale;
    std::string five = "Lives/5Lives.png";
    std::string four = "Lives/4Lives.png";
    std::string three = "Lives/3Lives.png";
    std::string two = "Lives/2Lives.png";
    std::string one = "Lives/1Lives.png";
    std::string zero = "Lives/0Lives.png";
};

#endif // HUD_H
