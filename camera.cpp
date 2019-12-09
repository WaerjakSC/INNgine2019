#include "camera.h"

Camera::Camera(float fov, float aspectRatio, float near, float far)
{
    mViewMatrix.setToIdentity();
    mProjectionMatrix.setToIdentity();

    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();
    setProjectionMatrix(fov, aspectRatio, near, far);
}

Camera::mat4 Camera::getProjectionMatrix() const
{
    return mProjectionMatrix;
}

Camera::mat4 Camera::getViewMatrix() const
{
    return mViewMatrix;
}
void Camera::setProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane)
{
    mProjectionMatrix.perspective(fov, aspect, nearPlane, farPlane);
}

void Camera::setPosition(const vec3 &position)
{
    mPosition = -position;
}
const gsl::Vector3D Camera::getRotation() const
{
    return std::get<2>(gsl::Matrix4x4::decomposed(mViewMatrix));
}

void Camera::setRotation(float pitch, float yaw)
{
    mPitch = -pitch;
    mYaw = -yaw;

    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();

    mPitchMatrix.rotateX(mPitch);
    mYawMatrix.rotateY(mYaw);
}
void Camera::calculateViewMatrix()
{
    mViewMatrix = mPitchMatrix * mYawMatrix;
    mViewMatrix.translate(mPosition);
    makeFrustum();
}
gsl::Vector3D Camera::position() const
{
    return mPosition;
}

gsl::Vector3D Camera::calculateMouseRay(const vec3 &viewportPoint, int height, int width)
{
    vec3 rayNDC{getNormalizedDeviceCoords(viewportPoint, height, width)};

    vec4 rayClip{rayNDC.x, rayNDC.y, -1.0, 1.0};

    gsl::Matrix4x4 projMatrix{mProjectionMatrix};
    projMatrix.inverse();

    vec4 rayEye{projMatrix * rayClip}; // Invert the perspective matrix to get to view/eye space (camera)
    rayEye = vec4{rayEye.x, rayEye.y, -1.0, 0.0};

    gsl::Matrix4x4 viewMatrix{mViewMatrix};
    viewMatrix.inverse();
    vec3 rayWorld{(viewMatrix * rayEye).getXYZ()};
    rayWorld.normalize();

    return rayWorld;
}
gsl::Vector3D Camera::getNormalizedDeviceCoords(const vec3 &mouse, int height, int width)
{
    float x{(2.0f * mouse.x) / width - 1.0f};
    float y{1.0f - (2.0f * mouse.y) / height};
    float z{mouse.z};
    return vec3{x, y, z}; // Normalised Device Coordinates range [-1:1, -1:1, -1:1]
}

void Camera::makeFrustum()
{
    Frustum result;
    mat4 vp{getViewMatrix() * getProjectionMatrix()};

    vec3 col1{vp[0], vp[1], vp[2]};
    vec3 col2{vp[4], vp[5], vp[6]};
    vec3 col3{vp[8], vp[9], vp[10]};
    vec3 col4{vp[12], vp[13], vp[14]};

    result.plane.left.normal = col4 + col1;
    result.plane.right.normal = col4 - col1;
    result.plane.bottom.normal = col4 + col2;
    result.plane.top.normal = col4 - col2;
    result.plane.near.normal = col3;
    result.plane.far.normal = col4 + col3;

    result.plane.left.distance = vp[15] + vp[3];
    result.plane.right.distance = vp[15] - vp[3];
    result.plane.bottom.distance = vp[15] + vp[7];
    result.plane.top.distance = vp[15] - vp[7];
    result.plane.near.distance = vp[11];
    result.plane.far.distance = vp[15] - vp[11];

    for (int i{0}; i < 6; i++) {
        float mag{1.0f / result.planes[i].normal.length()};
        result.planes[i].normal = result.planes[i].normal * mag;
        result.planes[i].distance *= mag;
    }
    mFrustum = result;
}
// Finds the corners where the planes p1, p2 and p3 intersect.
gsl::Vector3D Camera::Frustum::Intersection(Plane p1, Plane p2, Plane p3)
{
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

    float detD{D.determinant()};

    if (detD == 0) {
        return vec3{};
    }

    float detDx{Dx.determinant()};
    float detDy{Dy.determinant()};
    float detDz{Dz.determinant()};

    return vec3{detDx / detD, detDy / detD, detDz / detD};
}

// Calls Intersection function 8 times to find all corners in near & far plane of frustum.
void Camera::Frustum::GetCorners(const Frustum &f, std::vector<vec3> &outCorners)
{
    outCorners[0] = Intersection(f.plane.near, f.plane.top, f.plane.left);
    outCorners[1] = Intersection(f.plane.near, f.plane.top, f.plane.right);
    outCorners[2] = Intersection(f.plane.near, f.plane.bottom, f.plane.left);
    outCorners[3] = Intersection(f.plane.near, f.plane.bottom, f.plane.right);
    outCorners[4] = Intersection(f.plane.far, f.plane.top, f.plane.left);
    outCorners[5] = Intersection(f.plane.far, f.plane.top, f.plane.right);
    outCorners[6] = Intersection(f.plane.far, f.plane.bottom, f.plane.left);
    outCorners[7] = Intersection(f.plane.far, f.plane.bottom, f.plane.right);
}

float Camera::Frustum::Classify(const AABB &aabb, const Plane &plane)
{
    // maximum extent in direction of plane normal
    float r{fabsf(aabb.size.x * plane.normal.x) + fabsf(aabb.size.y * plane.normal.y) + fabsf(aabb.size.z * plane.normal.z)};
    vec3 center{aabb.transform.modelMatrix.getPosition() + aabb.origin};
    // signed distance between box center and plane
    float d{vec3::dot(plane.normal, center) + plane.distance};
    if (fabsf(d) < r) {
        return 0.0f;
    }
    else if (d < 0.0f) {
        return d + r;
    }
    return d - r;
}
bool Camera::Frustum::Intersects(const Frustum &f, const Sphere &s)
{
    vec3 sPosition{s.transform.modelMatrix.getPosition()};
    for (int i{0}; i < 6; i++) {
        vec3 normal{f.planes[i].normal};
        float dist{f.planes[i].distance};
        float side{vec3::dot(sPosition, normal) + dist};
        if (side < -s.radius) {
            return false;
        }
    }
    return true;
}
bool Camera::Frustum::Intersects(const Frustum &f, const AABB &aabb)
{
    for (int i{0}; i < 6; ++i) {
        if (Classify(aabb, f.planes[i]) < 0) {
            return false;
        }
    }
    return true;
}
