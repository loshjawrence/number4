#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    Intersection inters = Sample(xi, pdf);

    //Converet light sample weight to solid angle measure
    Vector3f LightVec = glm::normalize(inters.point - ref.point);

    float InvArea = *pdf;

    float NoL = glm::abs(glm::dot(inters.normalGeometric, -LightVec));

    //Exception Handling
    if(NoL != 0.0f)
        *pdf = glm::distance2(ref.point, inters.point) * InvArea / NoL;

    return inters;
}
