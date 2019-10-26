#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "isystem.h"
#include "pool.h"
#include <memory>

typedef gsl::Vector3D vec3;
class Registry;
class Transform;
class MovementSystem : public QObject, public ISystem {
    Q_OBJECT
    friend class RenderWindow;

private:
    Registry *registry;

    void updateModelMatrix(Transform &comp);
    gsl::Matrix4x4 getTRMatrix(Transform &comp);

public:
    MovementSystem();

    void update(float deltaTime = 0.016) override;
    void init();

    void updateTRS(Transform &comp);
    void updateEntity(GLuint eID);

    vec3 getAbsolutePosition(GLuint eID);
    vec3 getLocalPosition(GLuint eID);
    // ******** Position Setters ******** //
    void setAbsolutePosition(GLuint eID, vec3 position, bool signal = true);

    void setLocalPosition(GLuint eID, vec3 position, bool signal = true);
    void setLocalPosition(int eID, float xIn, float yIn, float zIn, bool signal = true);

    void setAbsolutePositionX(int eID, float xIn, bool signal);
    void setAbsolutePositionY(int eID, float yIn, bool signal);
    void setAbsolutePositionZ(int eID, float zIn, bool signal);
    void setLocalPositionX(int eID, float xIn, bool signal = true);
    void setLocalPositionY(int eID, float yIn, bool signal = true);
    void setLocalPositionZ(int eID, float zIn, bool signal = true);

    void moveX(GLuint eID, float xIn, bool signal = true);
    void moveY(GLuint eID, float yIn, bool signal = true);
    void moveZ(GLuint eID, float zIn, bool signal = true);
    // ******** Rotation Setters ******** //
    void setRotation(GLuint eID, vec3 rotation, bool signal = true);

    void setRotationX(int eID, float xIn, bool signal = true);
    void setRotationY(int eID, float yIn, bool signal = true);
    void setRotationZ(int eID, float zIn, bool signal = true);

    void rotateX(GLuint eID, float xIn, bool signal = true);
    void rotateY(GLuint eID, float yIn, bool signal = true);
    void rotateZ(GLuint eID, float zIn, bool signal = true);

    // ******** Scale Setters ******** //
    void setScale(int eID, vec3 scale, bool signal = true);

    void setScaleX(int eID, float xIn, bool signal = true);
    void setScaleY(int eID, float yIn, bool signal = true);
    void setScaleZ(int eID, float zIn, bool signal = true);

    void scaleX(GLuint eID, float xIn, bool signal = true);
    void scaleY(GLuint eID, float yIn, bool signal = true);
    void scaleZ(GLuint eID, float zIn, bool signal = true);

    void move(GLuint eID, const vec3 &moveDelta, bool signal = true);
    void scale(GLuint eID, const vec3 &scaleDelta, bool signal = true);
    void rotate(GLuint eID, const vec3 &rotDelta, bool signal = true);
    gsl::Vector3D getAbsoluteRotation(GLuint eID);
    gsl::Vector3D getRelativeRotation(GLuint eID);

signals:
    void positionChanged(GLuint eID, vec3 newPos, bool isGlobal);
    void scaleChanged(GLuint eID, vec3 newScale);
    void rotationChanged(GLuint eID, vec3 newRot);
};

#endif // MOVEMENTSYSTEM_H
