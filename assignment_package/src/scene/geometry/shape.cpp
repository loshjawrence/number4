#include "shape.h"
#include <QDateTime>
#include "Sphere.h"
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
    Intersection sample = this->Sample(xi, pdf);

//    const Sphere* this_shape = dynamic_cast<const Sphere*>(this);

//    if(this_shape != nullptr) {
//        return sample;
//    } else {
        Vector3f wi = glm::normalize( sample.point - ref.point );

        float coslight = glm::abs( glm::dot(sample.normalGeometric, -wi));

        float denominator = coslight * (1.f/(*pdf));

        if(denominator != 0.f) {
            *pdf = glm::distance2(ref.point, sample.point) / denominator;
        } else {
            *pdf = 0.f;
        }

        return sample;
//    }
}
