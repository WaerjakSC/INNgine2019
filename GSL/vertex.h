#ifndef VERTEX_H
#define VERTEX_H

#include "vector2d.h"
#include "vector3d.h"
typedef gsl::Vector3D vec3;
class Vertex {
public:
    Vertex();
    Vertex(float x, float y, float z, float r, float g, float b);
    Vertex(float x, float y, float z, float r, float g, float b, float s, float t);
    Vertex(vec3 a, vec3 b = vec3(0), gsl::Vector2D c = gsl::Vector2D(0));
    ~Vertex();

    //! Overloaded ostream operator which writes all vertex data on an open textfile stream
    friend std::ostream &operator<<(std::ostream &, const Vertex &);

    //! Overloaded ostream operator which reads all vertex data from an open textfile stream
    friend std::istream &operator>>(std::istream &, Vertex &);

    void set_xyz(GLfloat *xyz);
    void set_xyz(GLfloat x, GLfloat y, GLfloat z);
    void set_xyz(vec3 xyz_in);
    void set_rgb(GLfloat *rgb);
    void set_rgb(GLfloat r, GLfloat g, GLfloat b);
    void set_normal(GLfloat *normal);
    void set_normal(GLfloat x, GLfloat y, GLfloat z);
    void set_normal(vec3 normal_in);
    void set_st(GLfloat *st);
    void set_st(GLfloat s, GLfloat t);

    bool operator==(const Vertex &other) const {
        return mXYZ == other.mXYZ && mNormal == other.mNormal && mST == other.mST;
    }

    vec3 mXYZ;
    vec3 mNormal;
    gsl::Vector2D mST;
};
namespace std {
template <>
struct hash<Vertex> {
    size_t operator()(Vertex const &vertex) const {
        return ((hash<gsl::Vector3D>()(vertex.mXYZ) ^
                 (hash<gsl::Vector3D>()(vertex.mNormal) << 1)) >>
                1) ^
               (hash<gsl::Vector2D>()(vertex.mST) << 1);
    }
};
} // namespace std
#endif // VERTEX_H
