#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/lights/light.h>

enum GlossyMode
{
    NONE = 0, PHONG, BLINNPHONG, BECKMANN, TROWBRIDGEREITZ, COOKTORRANCE, GGX_SPEC
};

class Primitive;
class Material;
class Light;

class Scene
{
public:
    Scene();
    QList<std::shared_ptr<Primitive>> primitives;
    QList<std::shared_ptr<Material>> materials;
    QList<std::shared_ptr<Light>> lights;
    Camera camera;
    Film film;

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void CreateSpecularTestScene();
    void Clear();

    bool Intersect(const Ray& ray, Intersection* isect) const;

    float Roughness;
    float Opacity;
    GlossyMode GlossY;
};
