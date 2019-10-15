#include "components.h"
#include "mainwindow.h"

Component::~Component() {
}

void Input::keyPressEvent(QKeyEvent *event) {
    //    You get the keyboard input like this

    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mMainWindow->close();
    }
    if (event->key() == Qt::Key_Control)
        CTRL = true;
    if (event->key() == Qt::Key_L) {
        L = true;
    }
    if (event->key() == Qt::Key_F) {
        F = true;
    }
    if (event->key() == Qt::Key_W) {
        W = true;
    }
    if (event->key() == Qt::Key_S) {
        S = true;
    }
    if (event->key() == Qt::Key_D) {
        D = true;
    }
    if (event->key() == Qt::Key_A) {
        A = true;
    }
    if (event->key() == Qt::Key_Q) {
        Q = true;
    }
    if (event->key() == Qt::Key_E) {
        E = true;
    }
    if (event->key() == Qt::Key_Z) {
    }
    if (event->key() == Qt::Key_X) {
    }
    if (event->key() == Qt::Key_Up) {
        UP = true;
    }
    if (event->key() == Qt::Key_Down) {
        DOWN = true;
    }
    if (event->key() == Qt::Key_Left) {
        LEFT = true;
    }
    if (event->key() == Qt::Key_Right) {
        RIGHT = true;
    }
    if (event->key() == Qt::Key_U) {
    }
    if (event->key() == Qt::Key_O) {
    }
}

void Input::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W) {
        W = false;
    }
    if (event->key() == Qt::Key_Control)
        CTRL = false;
    if (event->key() == Qt::Key_L) {
        L = false;
    }
    if (event->key() == Qt::Key_F) {
        F = false;
    }
    if (event->key() == Qt::Key_S) {
        S = false;
    }
    if (event->key() == Qt::Key_D) {
        D = false;
    }
    if (event->key() == Qt::Key_A) {
        A = false;
    }
    if (event->key() == Qt::Key_Q) {
        Q = false;
    }

    if (event->key() == Qt::Key_E) {
        E = false;
    }

    if (event->key() == Qt::Key_Z) {
    }

    if (event->key() == Qt::Key_X) {
    }

    if (event->key() == Qt::Key_Up) {
        UP = false;
    }

    if (event->key() == Qt::Key_Down) {
        DOWN = false;
    }

    if (event->key() == Qt::Key_Left) {
        LEFT = false;
    }

    if (event->key() == Qt::Key_Right) {
        RIGHT = false;
    }

    if (event->key() == Qt::Key_U) {
    }

    if (event->key() == Qt::Key_O) {
    }
}

void Input::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        RMB = true;
    if (event->button() == Qt::LeftButton)
        LMB = true;
    if (event->button() == Qt::MiddleButton)
        MMB = true;
}

void Input::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        RMB = false;
    if (event->button() == Qt::LeftButton)
        LMB = false;
    if (event->button() == Qt::MiddleButton)
        MMB = false;
}

/*
void Input::wheelEvent(QWheelEvent *event){
    QPoint numDegrees = event->angleDelta() / 8;
    RenderWindow r;
    //if RMB, change the speed of the camera
    if (RMB) {
        if (numDegrees.y() < 1)
        r.setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
        r.setCameraSpeed(-0.001f);
    }
    event->accept();
}
*/

// Finds the corners where the planes p1, p2 and p3 intersect.
vec3 Frustum::Intersection(plane p1, plane p2, plane p3) {
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
void Frustum::GetCorners(const Frustum &f, vec3 *outCorners) {
    outCorners[0] = Intersection(f.planeType.near, f.planeType.top, f.planeType.left);
    outCorners[1] = Intersection(f.planeType.near, f.planeType.top, f.planeType.right);
    outCorners[2] = Intersection(f.planeType.near, f.planeType.bottom, f.planeType.left);
    outCorners[3] = Intersection(f.planeType.near, f.planeType.bottom, f.planeType.right);
    outCorners[4] = Intersection(f.planeType.far, f.planeType.top, f.planeType.left);
    outCorners[5] = Intersection(f.planeType.far, f.planeType.top, f.planeType.right);
    outCorners[6] = Intersection(f.planeType.far, f.planeType.bottom, f.planeType.left);
    outCorners[7] = Intersection(f.planeType.far, f.planeType.bottom, f.planeType.right);
}

bool Frustum::Intersects(const Frustum &f, const sphere &s) {
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

float Frustum::Classify(const aABB &aabb, const plane &plane) {
    // maximum extent in direction of plane normal
    float r = fabsf(aabb.size.x * plane.normal.x)
            + fabsf(aabb.size.y * plane.normal.y)
            + fabsf(aabb.size.z * plane.normal.z);
    // signed distance between box center and plane
    float d = plane.normal.dot(plane.normal, aabb.position) + plane.distance;
    if (fabsf(d) < r) {
        return 0.0f;
    }
    else if (d < 0.0f) {
        return d + r;
    }
    return d - r;
}

float Frustum::Classify(const oBB &obb, const plane &plane) {
    vec3 normal = MultiplyVector(plane.normal, obb.orientation);

    // maximum extent in direction of plane normal
    float r = fabsf(obb.size.x * normal.x)
            + fabsf(obb.size.y * normal.y)
            + fabsf(obb.size.z * normal.z);
    // signed distance between box center and plane
    float d = plane.normal.dot(plane.normal, obb.position)
            + plane.distance;
    // return signed distance
    if (fabsf(d) < r) {
        return 0.0f;
    }
    else if (d < 0.0f) {
        return d + r;
    }
    return d - r;
}
