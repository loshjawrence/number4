#include "squareplane.h"

float SquarePlane::Area() const
{
    //TODO
    return transform.getScale().x * transform.getScale().y;
}

bool SquarePlane::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void SquarePlane::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    //TODO: Compute tangent and bitangent

    glm::vec4 ta = transform.T() * glm::vec4(1,0,0,0);
    *tan = glm::normalize(Vector3f(ta.x, ta.y, ta.z));

    glm::vec4 bi = transform.T() * glm::vec4(0,1,0,0);
    *bit = glm::normalize(Vector3f(bi.x, bi.y, bi.z));

    //*bit = glm::normalize(glm::cross(*nor ,*tan));
}


Point2f SquarePlane::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(point.x + 0.5f, point.y + 0.5f);
}

Intersection SquarePlane::Sample(const Point2f &xi, Float *pdf) const
{
    Intersection inter;

    //A SquarePlane is assumed to have a radius of 1 and a center of <0,0,0>.
    Point2f pt = Point2f(xi.x -0.5f, xi.y -0.5f);

    glm::vec4 WSP = transform.T() * glm::vec4(pt.x, pt.y, 0.0f,1.0f);

    Point3f WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);

    inter.point = WorldSpacePoint;
    inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,0,1));

    *pdf = 1.0f / Area();
    return inter;
}
