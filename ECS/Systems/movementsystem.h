#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "pool.h"
#include <QObject>
#include <memory>
class MovementSystem : public QObject {
    Q_OBJECT
public:
    MovementSystem();

    void update();

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
    gsl::Matrix4x4 multiplyByParent(int eID, int pID);

    void updateMatrix(Transform &comp);

    std::vector<int> &children(const int eID) const;
    void addChild(const int eID, const GLuint childID);
    void removeChild(const int eID, const GLuint childID);
    void iterateChildren(int eID);

    gsl::Vector3D getPosition(int eID);
    gsl::Vector3D getRelativePosition(int eID);
    // ******** Position Setters ******** //
    void setPosition(int eID, gsl::Vector3D position);
    void setPosition(int eID, float xIn, float yIn, float zIn);

    void setPositionX(int eID, float xIn);
    void setPositionY(int eID, float yIn);
    void setPositionZ(int eID, float zIn);

    void moveX(int eID, float xIn);
    void moveY(int eID, float yIn);
    void moveZ(int eID, float zIn);
    // ******** Rotation Setters ******** //
    void setRotation(int eID, gsl::Vector3D rotation);

    void setRotationX(int eID, float xIn);
    void setRotationY(int eID, float yIn);
    void setRotationZ(int eID, float zIn);

    // ******** Scale Setters ******** //
    void setScale(int eID, gsl::Vector3D scale);

    void setScaleX(int eID, float xIn);
    void setScaleY(int eID, float yIn);
    void setScaleZ(int eID, float zIn);

signals:
    void positionChanged(gsl::Vector3D newPos);
    void scaleChanged(gsl::Vector3D newScale);
    void rotationChanged(gsl::Vector3D newRot);

private:
    std::shared_ptr<Pool<Transform>> mTransforms;
};

#endif // MOVEMENTSYSTEM_H
