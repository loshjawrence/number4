#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO
    if (this->twoSided || SameHemisphere(w,isect.normalGeometric)) {
        return emittedLight;
    } else {
        return Color3f(0.f);
    }
}

// Given a point of intersection on some surface in the scene and a pair
// of uniform random variables, generate a sample point on the surface
// of our shape and evaluate the light emitted along the ray from
// our Shape's surface to the reference point.
Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf) const {
    Intersection lightsample = this->shape->Sample(ref,xi,pdf);

    *wi = glm::normalize(lightsample.point - ref.point);

//    bool wrongside = !(this->twoSided) && !SameHemisphere(-1.f * (*wi),lightsample.normalGeometric);
//    bool samepoint =  fabs(lightsample.point.x - ref.point.x) < 0.0001f
//                   && fabs(lightsample.point.y - ref.point.y) < 0.0001f
//                   && fabs(lightsample.point.z - ref.point.z) < 0.0001f;

//    if( (*pdf) == 0.f || samepoint || wrongside) {
    if( (*pdf) == 0.f ) {
        *pdf == 0.f;
        return Color3f(0.f);
    }

    return emittedLight;
}

float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    //TODO
    float pdfli = 0.f;
    Intersection closest = Intersection();
    Ray wiWray = ref.SpawnRay(wi);
    this->shape->Intersect(wiWray,&closest);

    if(closest.objectHit == nullptr || closest.objectHit->GetAreaLight() != this) {
       //BxDF sample didn't hit the light. pdfli still 0
    } else {
        //else return the pdf due to our intersection with the light
        float coslight = glm::abs( glm::dot(closest.normalGeometric, -wi));
        float denominator = coslight * closest.objectHit->shape->Area();
        if(denominator != 0.f) {
            pdfli = glm::distance2(ref.point, closest.point) / denominator;
        }
    }

    return pdfli;
}
