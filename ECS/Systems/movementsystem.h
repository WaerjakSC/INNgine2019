#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "pool.h"
#include <QObject>
#include <memory>

typedef gsl::Vector3D vec3;
class Registry;
class Transform;
class MovementSystem : public QObject {
    Q_OBJECT
public:
    MovementSystem();

    void update();
    void init();

    void updateTRS(Transform &comp);
    void updateEntity(GLuint eID);

    vec3 getAbsolutePosition(int eID);
    vec3 getRelativePosition(int eID);
    // ******** Position Setters ******** //
    void setAbsolutePosition(GLuint eID, vec3 position, bool signal = true);

    void setPosition(GLuint eID, vec3 position, bool signal = true);
    void setPosition(int eID, float xIn, float yIn, float zIn, bool signal = true);

    void setPositionX(int eID, float xIn, bool signal = true);
    void setPositionY(int eID, float yIn, bool signal = true);
    void setPositionZ(int eID, float zIn, bool signal = true);

    void moveX(int eID, float xIn);
    void moveY(int eID, float yIn);
    void moveZ(int eID, float zIn);
    // ******** Rotation Setters ******** //
    void setRotation(GLuint eID, vec3 rotation, bool signal = true);

    void setRotationX(int eID, float xIn, bool signal = true);
    void setRotationY(int eID, float yIn, bool signal = true);
    void setRotationZ(int eID, float zIn, bool signal = true);

    void rotateX(GLuint eID, float xIn, bool signal);
    void rotateY(GLuint eID, float yIn, bool signal);
    void rotateZ(GLuint eID, float zIn, bool signal);

    // ******** Scale Setters ******** //
    void setScale(int eID, vec3 scale, bool signal = true);

    void setScaleX(int eID, float xIn, bool signal = true);
    void setScaleY(int eID, float yIn, bool signal = true);
    void setScaleZ(int eID, float zIn, bool signal = true);

    void scaleX(GLuint eID, float xIn, bool signal);
    void scaleY(GLuint eID, float yIn, bool signal);
    void scaleZ(GLuint eID, float zIn, bool signal);
signals:
    void positionChanged(GLuint eID, vec3 newPos);
    void scaleChanged(GLuint eID, vec3 newScale);
    void rotationChanged(GLuint eID, vec3 newRot);

private:
    std::shared_ptr<Pool<Transform>> mTransforms;
    Registry *registry;
    void setPositionPrivate(GLuint eID, vec3 position);
    void setRotationPrivate(GLuint eID, vec3 rotation);
    void updateModelMatrix(Transform &comp);
    gsl::Matrix4x4 getTRMatrix(Transform &comp);
};

#endif // MOVEMENTSYSTEM_H
