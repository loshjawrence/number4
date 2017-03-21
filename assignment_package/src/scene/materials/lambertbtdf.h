#pragma once
#include "bsdf.h"

class LambertBTDF : public BxDF
{
public:
    LambertBTDF(const Color3f &R, const float &opacity)
        : BxDF(BxDFType(BSDF_DIFFUSE | BSDF_TRANSMISSION)), R(R), opacity(opacity) {}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;


  private:
    const Color3f R; // The energy scattering coefficient of this BRDF (i.e. its color)
    const float opacity;
};
