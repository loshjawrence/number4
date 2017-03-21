#pragma once
#include "bsdf.h"

class LambertBRDF : public BxDF
{
public:
    LambertBRDF(const Color3f &R)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;


  private:
    const Color3f R; // The energy scattering coefficient of this BRDF (i.e. its color)
};

class OrenNayarBRDF : public BxDF
{
public:
    OrenNayarBRDF(const Color3f &R, float sigma)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {
        sigma = glm::radians(sigma);//std dev of the microfacet orientation angle
        float sigma2 = sigma * sigma;
        A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
        B = 0.45f * sigma2 / (sigma2 + 0.09f);
    }

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

  private:
    const Color3f R; // The energy scattering coefficient of this BRDF (i.e. its color)
    float A;
    float B;
};

class LambertBTDF : public BxDF
{
public:
    LambertBTDF(const Color3f &R)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;


  private:
    const Color3f R; // The energy scattering coefficient of this BRDF (i.e. its color)
};
