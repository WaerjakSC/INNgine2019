#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H


class MovementSystem
{
public:
    MovementSystem();
    void update();

    /*
     * Remove from transformcomponent and implement here later
     *
    void setPosition(gsl::Vector3D &position);
    void setPosition(float xIn, float yIn, float zIn);
    void setScale(gsl::Vector3D &scale);
    void setRotation(gsl::Vector3D &rotation);

    gsl::Matrix4x4 multiplyByParent(gsl::Matrix4x4 matrixIn);
    void updateMatrix();
    */
};

#endif // MOVEMENTSYSTEM_H
