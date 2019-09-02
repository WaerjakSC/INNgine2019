#include "light.h"
#include "innpch.h"
#include "renderwindow.h"

Light::Light(std::string name) : GameObject(name) {

    auto *meshObject = new MeshComponent();
    meshObject->setMesh(new meshData());
    meshObject->getMesh()->mVertices.insert(meshObject->getMesh()->mVertices.end(),
                                            {
                                                //Vertex data - normals not correct
                                                Vertex{gsl::Vector3D(-0.5f, -0.5f, 0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.f, 0.f)},  //Left low
                                                Vertex{gsl::Vector3D(0.5f, -0.5f, 0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(1.f, 0.f)},   //Right low
                                                Vertex{gsl::Vector3D(0.0f, 0.5f, 0.0f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.5f)},  //Top
                                                Vertex{gsl::Vector3D(0.0f, -0.5f, -0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.5f)} //Back low
                                            });

    meshObject->getMesh()->mIndices.insert(meshObject->getMesh()->mIndices.end(),
                                           {0, 1, 2,
                                            1, 3, 2,
                                            3, 0, 2,
                                            0, 3, 1});
    meshObject->mMaterial.setTextureUnit(0);
    meshObject->mMaterial.mObjectColor = gsl::Vector3D(0.1f, 0.1f, 0.8f);
}
