#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "pool.h"
#include <QObject>
#include <memory>
class Registry;
class MovementSystem : public QObject {
    Q_OBJECT
public:
    MovementSystem();

    void update();
    void init();
    /**
     * @brief Check if Transform component (or rather its entity) has a parent object
     * @param eID
     * @return
     */
    bool hasParent(int eID);
    /**
     * @brief Remove the entity's old parent (if it had one) and set it to the given parentID. If parentID == -1, simply removes the parent.
     * @param eID
     * @param parentID
     */
    void setParent(int eID, int parentID);
    gsl::Matrix4x4 multiplyByParent(GLuint eID, GLuint pID);

    void updateMatrix(GLuint eID, Transform &comp);
    void updateEntity(GLuint eID);

    void iterateChildren(int eID);

    gsl::Vector3D getPosition(int eID);
    gsl::Vector3D getRelativePosition(int eID);
    // ******** Position Setters ******** //
    void setPosition(GLuint eID, gsl::Vector3D position, bool signal = true);
    void setPosition(int eID, float xIn, float yIn, float zIn, bool signal = true);

    void setPositionX(int eID, float xIn, bool signal = true);
    void setPositionY(int eID, float yIn, bool signal = true);
    void setPositionZ(int eID, float zIn, bool signal = true);

    void moveX(int eID, float xIn);
    void moveY(int eID, float yIn);
    void moveZ(int eID, float zIn);
    // ******** Rotation Setters ******** //
    void setRotation(GLuint eID, gsl::Vector3D rotation, bool signal = false);

    void setRotationX(int eID, float xIn, bool signal = true);
    void setRotationY(int eID, float yIn, bool signal = true);
    void setRotationZ(int eID, float zIn, bool signal = true);

    void rotateX(GLuint eID, float xIn, bool signal);
    void rotateY(GLuint eID, float yIn, bool signal);
    void rotateZ(GLuint eID, float zIn, bool signal);

    // ******** Scale Setters ******** //
    void setScale(int eID, gsl::Vector3D scale, bool signal = true);

    void setScaleX(int eID, float xIn, bool signal = true);
    void setScaleY(int eID, float yIn, bool signal = true);
    void setScaleZ(int eID, float zIn, bool signal = true);

    void scaleX(GLuint eID, float xIn, bool signal);
    void scaleY(GLuint eID, float yIn, bool signal);
    void scaleZ(GLuint eID, float zIn, bool signal);
signals:
    void positionChanged(GLuint eID, gsl::Vector3D newPos);
    void scaleChanged(GLuint eID, gsl::Vector3D newScale);
    void rotationChanged(GLuint eID, gsl::Vector3D newRot);

private:
    std::shared_ptr<Pool<Transform>> mTransforms;
    Registry *registry;
    void setPositionPrivate(GLuint eID, vec3 position);
    void setRotationPrivate(GLuint eID, gsl::Vector3D rotation);
};

#endif // MOVEMENTSYSTEM_H
