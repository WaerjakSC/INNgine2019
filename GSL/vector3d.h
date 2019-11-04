#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "gltypes.h"
#include <QDebug>
#include <QVector3D>
#include <cmath>
#include <iostream>

namespace gsl {

class Vector3D {
public:
    //Constructors
    Vector3D(GLfloat x_in = 0.f, GLfloat y_in = 0.f, GLfloat z_in = 0.f);
    Vector3D(const int v);
    Vector3D(const double v);

    //Operators
    const Vector3D &operator=(const Vector3D &rhs); // v = v
    Vector3D operator+(const Vector3D &rhs) const;  // v + v
    Vector3D operator-(const Vector3D &rhs) const;  // v - v
    Vector3D operator/(const Vector3D &rhs) const;
    Vector3D &operator+=(const Vector3D &rhs);     // v += v
    Vector3D &operator-=(const Vector3D &rhs);     // v -= v
    Vector3D operator-() const;                    // -v
    Vector3D operator*(GLfloat rhs) const;         // v * f
    Vector3D operator^(const Vector3D &rhs) const; // v x v  - cross product
    bool operator==(const Vector3D &compared) const;
    bool operator!=(const Vector3D &compared) const;
    GLfloat &at(GLint index);

    //Functions
    GLfloat length() const;
    void normalize();
    Vector3D normalized();
    static Vector3D multiply(const Vector3D &lhs, const Vector3D &rhs);
    static Vector3D cross(const Vector3D &v1, const Vector3D &v2);
    static GLfloat dot(const Vector3D &v1, const Vector3D &v2);

    void rotate(const Vector3D &euler);
    void rotateX(GLfloat angle);
    void rotateY(GLfloat angle);
    void rotateZ(GLfloat angle);

    //Getters and setters
    GLfloat getX() const;
    void setX(const GLfloat &value);

    GLfloat getY() const;
    void setY(const GLfloat &value);

    GLfloat getZ() const;
    void setZ(const GLfloat &value);

    GLfloat *xP();
    GLfloat *yP();
    GLfloat *zP();

    //    float vecArray[3];
    GLfloat operator[](int i) const {
        switch (i) {
        case 0:
            return x;

        case 1:
            return y;

        case 2:
            return z;
        }
        return x;
    }

    //Friend functions
    friend std::ostream &operator<<(std::ostream &output, const Vector3D &rhs) {
        output << "X = " << rhs.x << ", Y = " << rhs.y << ", Z = " << rhs.z;
        return output;
    }

    //OEF: Adding these again to be able to debug easier. cout does not print until program is finished!
    friend QDebug operator<<(QDebug out, const Vector3D &rhs) {
        out << "(" << rhs.x << "," << rhs.y << "," << rhs.z << ")";
        return out;
    } //    QVector3D getQVector() const;   //for use with qDebug()
    friend std::istream &operator>>(std::istream &is, gsl::Vector3D &v) {
        // char temp, temp2, temp3, temp4;

        is >> v.x >> v.z >> v.y;
        return is;
    }
    QVector3D getQVector() const {
        return QVector3D(x, y, z);
    } //for use with qDebug()

    //private:  //Making these public to avoid having to do things like setY(getY()+3) etc.
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

} // namespace gsl
namespace std {
template <>
struct hash<gsl::Vector3D> {
    size_t operator()(gsl::Vector3D const &v) const {
        return ((hash<GLfloat>()(v.x) ^
                 (hash<GLfloat>()(v.y) << 1)) >>
                1) ^
               (hash<GLfloat>()(v.z) << 1);
    }
};
} // namespace std
#endif // VECTOR3D_H
