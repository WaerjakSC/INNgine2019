#include "camera.h"
#include "innpch.h"

Camera::Camera() {
    mViewMatrix.setToIdentity();
    mProjectionMatrix.setToIdentity();

    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();
}

void Camera::pitch(float degrees) {
    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}
void Camera::setPitch(float newPitch) {
    mPitch = newPitch;
    updateForwardVector();
}
void Camera::setYaw(float newYaw) {
    mPitch = newYaw;
    updateForwardVector();
}
/**
 * @brief go to location
 * @param target
 */
void Camera::goTo(vec3 target) {
    vec3 targetDistance{0, 0, 5};
    const vec3 position = target + targetDistance;
    const vec3 direction = (position - target).normalized();

    mYaw = gsl::rad2degf(gsl::atan2(direction.x, direction.z));
    mPitch = gsl::rad2degf(gsl::asin(-direction.y));
    mPosition = position;
    updateForwardVector();
}
void Camera::yaw(float degrees) {
    // rotate around mUp
    mYaw -= degrees;
    updateForwardVector();
}

void Camera::updateRightVector() {
    mRight = mForward ^ mUp;
    mRight.normalize();
    //    qDebug() << "Right " << mRight;
}

void Camera::updateForwardVector() {
    mRight = vec3(1.f, 0.f, 0.f);
    mRight.rotateY(mYaw);
    mRight.normalize();
    mUp = vec3(0.f, 1.f, 0.f);
    mUp.rotateX(mPitch);
    mUp.normalize();
    mForward = mUp ^ mRight;

    updateRightVector();
}

void Camera::update() {
    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();

    mPitchMatrix.rotateX(-mPitch);
    mYawMatrix.rotateY(-mYaw);

    mPosition -= mForward * mSpeed;

    mViewMatrix = mPitchMatrix * mYawMatrix;
    mViewMatrix.translate(-mPosition);
}

gsl::Matrix4x4 Camera::getProjectionMatrix() const {
    return mProjectionMatrix;
}

gsl::Matrix4x4 Camera::getViewMatrix() const {
    return mViewMatrix;
}

void Camera::setPosition(const vec3 &position) {
    mPosition = position;
}

void Camera::setSpeed(float speed) {
    mSpeed = speed;
}

void Camera::updateHeight(float deltaHeight) {
    mPosition.y += deltaHeight;
}

void Camera::moveRight(float delta) {
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when strafing
    //should be fixed through correct right calculations!
    vec3 right = mRight;
    right.y = 0.f;
    mPosition += right * delta;
}

vec3 Camera::position() const {
    return mPosition;
}

vec3 Camera::up() const {
    return mUp;
}

vec3 Camera::forward() const {
    return mForward;
}
vec3 Camera::calculateMouseRay(const vec3 &viewportPoint, int height, int width) {
    vec3 rayNDC = getNormalizedDeviceCoords(viewportPoint, height, width);

    vec4 rayClip = vec4(rayNDC.x, rayNDC.y, -1.0, 1.0);

    gsl::Matrix4x4 projMatrix = mProjectionMatrix;
    projMatrix.inverse();

    vec4 rayEye = projMatrix * rayClip; // Invert the perspective matrix to get to view/eye space (camera)
    rayEye = vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    gsl::Matrix4x4 viewMatrix = mViewMatrix;
    viewMatrix.inverse();
    vec3 rayWorld = (viewMatrix * rayEye).getXYZ();
    rayWorld.normalize();

    return rayWorld;
}
vec3 Camera::getNormalizedDeviceCoords(const vec3 &mouse, int height, int width) {
    float x = (2.0f * mouse.x) / width - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / height;
    float z = mouse.z;
    return vec3(x, y, z); // Normalised Device Coordinates range [-1:1, -1:1, -1:1]
}

Camera::Frustum Camera::getFrustum() {
    Frustum result;
    mat4 vp = getViewMatrix() * getProjectionMatrix();

    vec3 col1{vp.getFloat(0), vp.getFloat(1), vp.getFloat(2)};
    vec3 col2{vp.getFloat(4), vp.getFloat(5), vp.getFloat(6)};
    vec3 col3{vp.getFloat(8), vp.getFloat(9), vp.getFloat(10)};
    vec3 col4{vp.getFloat(12), vp.getFloat(13), vp.getFloat(14)};

    result.planeType.left.normal = col4 + col1;
    result.planeType.right.normal = col4 - col1;
    result.planeType.bottom.normal = col4 + col2;
    result.planeType.top.normal = col4 - col2;
    result.planeType.near.normal = col3;
    result.planeType.far.normal = col4 + col3;

    result.planeType.left.distance = vp.getFloat(15) + vp.getFloat(3);
    result.planeType.right.distance = vp.getFloat(15) - vp.getFloat(3);
    result.planeType.bottom.distance = vp.getFloat(15) + vp.getFloat(7);
    result.planeType.top.distance = vp.getFloat(15) - vp.getFloat(7);
    result.planeType.near.distance = vp.getFloat(11);
    result.planeType.far.distance = vp.getFloat(15) - vp.getFloat(11);

    for (int i = 0; i < 6; i++) {
        float mag = 1.0f /
                    result.planes[i].normal.length();
        result.planes[i].normal =
            result.planes[i].normal * mag;
        result.planes[i].distance *= mag;
    }
    return result;
}

// Finds the corners where the planes p1, p2 and p3 intersect.
vec3 Camera::Frustum::Intersection(plane p1, plane p2, plane p3) {
    mat3 D{p1.normal.x, p2.normal.x, p3.normal.x,
           p1.normal.y, p2.normal.y, p3.normal.y,
           p1.normal.z, p2.normal.z, p3.normal.z};

    vec3 A(-p1.distance, -p2.distance, -p3.distance);

    mat3 Dx{A.x, A.y, A.z,
            p1.normal.y, p2.normal.y, p3.normal.y,
            p1.normal.z, p2.normal.z, p3.normal.z};
    mat3 Dy{p1.normal.x, p2.normal.x, p3.normal.x,
            A.x, A.y, A.z,
            p1.normal.z, p2.normal.z, p3.normal.z};
    mat3 Dz{p1.normal.x, p2.normal.x, p3.normal.x,
            p1.normal.y, p2.normal.y, p3.normal.y,
            A.x, A.y, A.z};

    float detD = D.determinant();

    if (detD == 0) {
        return vec3();
    }

    float detDx = Dx.determinant();
    float detDy = Dy.determinant();
    float detDz = Dz.determinant();

    return vec3(detDx / detD, detDy / detD, detDz / detD);
}

// Calls Intersection function 8 times to find all corners in near & far plane of frustum.
void Camera::Frustum::GetCorners(const Frustum &f, vec3 *outCorners) {
    outCorners[0] = Intersection(f.planeType.near, f.planeType.top, f.planeType.left);
    outCorners[1] = Intersection(f.planeType.near, f.planeType.top, f.planeType.right);
    outCorners[2] = Intersection(f.planeType.near, f.planeType.bottom, f.planeType.left);
    outCorners[3] = Intersection(f.planeType.near, f.planeType.bottom, f.planeType.right);
    outCorners[4] = Intersection(f.planeType.far, f.planeType.top, f.planeType.left);
    outCorners[5] = Intersection(f.planeType.far, f.planeType.top, f.planeType.right);
    outCorners[6] = Intersection(f.planeType.far, f.planeType.bottom, f.planeType.left);
    outCorners[7] = Intersection(f.planeType.far, f.planeType.bottom, f.planeType.right);
}

bool Camera::Frustum::Intersects(const Frustum &f, const sphere &s) {
    for (int i = 0; i < 6; i++) {
        vec3 normal = f.planes[i].normal;
        float dist = f.planes[i].distance;
        float side = s.position.dot(s.position, normal) + dist;
        if (side < -s.radius) {
            return false;
        }
    }
    return true;
}

float Camera::Frustum::Classify(const aABB &aabb, const plane &plane) {
    // maximum extent in direction of plane normal
    float r = fabsf(aabb.size.x * plane.normal.x) + fabsf(aabb.size.y * plane.normal.y) + fabsf(aabb.size.z * plane.normal.z);
    // signed distance between box center and plane
    float d = plane.normal.dot(plane.normal, aabb.origin) + plane.distance;
    if (fabsf(d) < r) {
        return 0.0f;
    } else if (d < 0.0f) {
        return d + r;
    }
    return d - r;
}

//float Camera::Frustum::Classify(const oBB &obb, const plane &plane) {
//    vec3 normal = MultiplyVector(plane.normal, obb.orientation);

//    // maximum extent in direction of plane normal
//    float r = fabsf(obb.size.x * normal.x)
//            + fabsf(obb.size.y * normal.y)
//            + fabsf(obb.size.z * normal.z);
//    // signed distance between box center and plane
//    float d = plane.normal.dot(plane.normal, obb.position)
//            + plane.distance;
//    // return signed distance
//    if (fabsf(d) < r) {
//        return 0.0f;
//    }
//    else if (d < 0.0f) {
//        return d + r;
//    }
//    return d - r;
//}

bool Camera::Frustum::Intersects(const Frustum &f, const aABB &aabb) {
    for (int i = 0; i < 6; ++i) {
        if (Classify(aabb, f.planes[i]) < 0) {
            return false;
        }
    }
    return true;
}

//bool Camera::Frustum::Intersects(const Frustum &f, const oBB &obb) {
//    for (int i = 0; i < 6; ++i) {
//        if (Classify(obb, f.planes[i]) < 0) {
//            return false;
//        }
//    }
//    return true;
//}
