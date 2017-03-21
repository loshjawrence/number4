#include "disc.h"

float Disc::Area() const
{
    //TODO
    return Pi * transform.getScale().x * transform.getScale().x; //assume uniform scaling
}


Intersection Disc::Sample(const Point2f &xi, Float *pdf) const {
    *pdf = 1.f / Area();

    Intersection sample = Intersection();
    sample.normalGeometric      = glm::normalize(transform.invTransT() * Normal3f(0,0,1));

    float r = std::sqrt(xi.x);
    float theta = 2.f * Pi * xi.y;
    glm::vec4 p                 = transform.T() * glm::vec4(r * std::cos(theta), r * std::sin(theta), 0.f, 1.f);
    sample.point = Vector3f(p.x, p.y, p.z);

    return sample;
}

bool Disc::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 1.f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void Disc::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    *tan = glm::normalize(transform.T3() * Vector3f(1,0,0));
    *bit = glm::normalize(transform.T3() * Vector3f(0,1,0));
}


Point2f Disc::GetUVCoordinates(const Point3f &point) const
{
    return glm::vec2((point.x + 1)/2.f, (point.y + 1)/2.f);
}
