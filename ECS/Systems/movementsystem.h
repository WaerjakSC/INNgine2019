#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "core.h"
#include "isystem.h"
#include "pool.h"

class Registry;
class MovementSystem : public QObject, public ISystem {
    Q_OBJECT
    friend class RenderWindow;
    using vec3 = gsl::Vector3D;

public:
    MovementSystem();

    void update(DeltaTime = 0.016) override;
    void init();
    /**
     * @brief getAbsolutePosition Get the absolute position of the entity from the entity's model matrix.
     * @param eID
     * @return vec3 containing the XYZ position of the entity
     */
    vec3 getAbsolutePosition(GLuint eID);
    /**
    * @brief getLocalPosition Get relative position of object (null-vector if no parent)
    * @param eID
    * @return localPosition vector from an entity's transform component
    */
    vec3 getLocalPosition(GLuint eID);
    /**
     * @brief getAbsoluteRotation get the absolute rotation of an object from the decomposed model matrix
     * @param eID
     * @return vec3 containing euler rotation values
     */
    vec3 getAbsoluteRotation(GLuint eID);
    /**
     * @brief getRelativeRotation simple function to grab the localRotation vector from an entity's transform component
     * @param eID
     * @return localRotation vector from an entity's transform component
     */
    vec3 getRelativeRotation(GLuint eID);
    // ******** Position Setters ******** //
    /**
     * @brief setAbsolutePosition position an entity according to its global position
     *  rather than its position relative to a parent
     * @param eID
     * @param position New position
     * @param signal decides whether to push the change in position on to the editor GUI
     * In cases where the editor GUI instigated the change, this would want to be false to avoid unnecessary looping
     */
    void setAbsolutePosition(GLuint eID, vec3 position, bool signal = true);
    /**
     * @brief setLocalPosition set the local position of the object, to be updated in the next update tick
     * @param eID
     * @param position New position
     * @param signal decides whether to push the change in position on to the editor GUI
     * In cases where the editor GUI instigated the change, this would want to be false to avoid unnecessary looping
     */
    void setLocalPosition(GLuint eID, vec3 position, bool signal = true);
    /**
     * @copydoc MovementSystem::setLocalPosition(GLuint,vec3,bool)
     */
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
    /**
     * @brief move Moves the entity by the delta given
     * @param eID
     * @param moveDelta
     * @param signal
     */
    void move(GLuint eID, const vec3 &moveDelta, bool signal = true);
    /**
     * @brief scale Scales the entity by the delta given
     * @param eID
     * @param scaleDelta
     * @param signal
     */
    void scale(GLuint eID, const vec3 &scaleDelta, bool signal = true);
    /**
     * @brief rotate Rotates the entity by the delta given
     * @param eID
     * @param rotDelta
     * @param signal
     */
    void rotate(GLuint eID, const vec3 &rotDelta, bool signal = true);

public slots:
    /**
     * @brief updateAABBTransform Gets a list of all AABB colliders and updates them to follow the entity according to their offset
     * @param entity Belonging entity
     */
    void updateAABBTransform(GLuint entity);
    /**
     * @brief updateSphereTransform Gets a list of all Sphere colliders and updates them to follow the entity according to their offset
     * @param entity Belonging entity
     */
    void updateSphereTransform(GLuint entity);

signals:
    void positionChanged(GLuint eID, vec3 newPos, bool isGlobal);
    void scaleChanged(GLuint eID, vec3 newScale);
    void rotationChanged(GLuint eID, vec3 newRot);

private:
    Registry *registry;
    /**
     * @brief updateModelMatrix Update the entity's model matrix if it's outdated, otherwise does nothing
     * @param eID
     */
    void updateModelMatrix(GLuint eID);
    void updateModelMatrix(Transform &comp);
    void updateColliders(GLuint eID);

    /**
     * @brief getTRMatrix Get the Translation and Rotation matrices of a component,
     * multiplied by its parent TR matrices if it has a parent.
     * We don't want to inherit the scale of the parent, since that can lead to wrong scaling for the child object.
     * @param comp
     * @return
     */
    gsl::Matrix4x4 getTRMatrix(const Transform &comp);

    void updateTRS(Transform &comp);
    /**
     * @brief updateTS Update the transform component belonging to the AABB component given
     * @param comp
     */
    void updateTS(AABB &comp);
    /**
     * @brief updateTS Update the transform component belonging to the Sphere component given
     * @param comp
     */
    void updateTS(Sphere &comp);

    void updateBillBoardTransform(GLuint entity);
    void updateColliderTransformPrivate(Collision &col, const Transform &trans);

    void updateBillBoardTransformPrivate(const BillBoard &billboard, Transform &transform, const Material &mat);
};

#endif // MOVEMENTSYSTEM_H
