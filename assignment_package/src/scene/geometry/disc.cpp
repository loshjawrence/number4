#include "disc.h"
#include "warpfunctions.h"

float Disc::Area() const
{
    //TODO
    return Pi * transform.getScale().x * transform.getScale().y;
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
    //TODO: Compute tangent and bitangent

    glm::vec4 ta = transform.T() * glm::vec4(1,0,0,0);
    glm::vec4 bi = transform.T() * glm::vec4(0,1,0,0);

    *tan = Vector3f(glm::normalize(Vector3f(ta.x, ta.y, ta.z)));
    *bit = Vector3f(glm::normalize(Vector3f(bi.x, bi.y, bi.z)));
}


Point2f Disc::GetUVCoordinates(const Point3f &point) const
{
    return glm::vec2((point.x + 1)/2.f, (point.y + 1)/2.f);
}

Intersection Disc::Sample(const Point2f &xi, Float *pdf) const
{
    Intersection inter;

    //A Disc is assumed to have a radius of 1 and a center of <0,0,0>.
    Point3f pt = WarpFunctions::squareToDiskConcentric(xi);

    glm::vec4 WSP = transform.T() * glm::vec4(pt.x, pt.y, pt.z,1.0f);

    Point3f WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);

    inter.point = WorldSpacePoint;
    inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,0,1));

    *pdf = 1 / Area();
    return inter;
}
