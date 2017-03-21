#include "lambertbtdf.h"
#include <warpfunctions.h>

Color3f LambertBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return InvPi * (R*opacity + Color3f(1.0f ,1.0f ,1.0f) * (1.0f - opacity));
}

Color3f LambertBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO
    *wi = glm::normalize( WarpFunctions::squareToHemisphereCosine(u));
    *pdf = Pdf(wo, *wi);

    //Out->in handling
    if(CosTheta(wo) > 0.0f)
        *wi = -*wi;

    return f(wo, *wi);
}

float LambertBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return WarpFunctions::squareToHemisphereCosinePDF(wi);
}

