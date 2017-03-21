#include "lambertbrdf.h"
#include <warpfunctions.h>

Color3f LambertBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return R * InvPi;
}

Color3f LambertBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO
    *wi = WarpFunctions::squareToHemisphereCosine(u);
    *pdf = LambertBRDF::Pdf(wo,*wi);
    return LambertBRDF::f(wo,*wi);
}

float LambertBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return WarpFunctions::squareToHemisphereCosinePDF(wi);
}

////Oren-Nayar diffuse reflection
//Color3f OrenNayarBRDF::f(const Vector3f &wo, const Vector3f &wi) const
//{
//    //TODO
//    float sinThetaI = BSDF::SinTheta(wi);
//    float sinThetaO = BSDF::SinTheta(wo);
//
//    //cos term of Oren-Nayar model
//    float maxCos = 0.f;
//    if(sinThetaI > 0.0001f && sinThetaO > 0.0001f) {
//        float sinPhiI = BSDF::SinPhi(wi), cosPhiI = BSDF::CosPhi(wi);
//        float sinPhiO = BSDF::SinPhi(wo), cosPhiO = BSDF::CosPhi(wo);
//
//        float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
//        maxCos = std::max(0.f, dCos);
//    }
//
//    //sin and tan terms of Oren-Nayar model
//    float sinAlpha, tanBeta;
//    if(AbsCosTheta(wi) > AbsCosTheta(wo)) {
//        sinAlpha = sinThetaO;
//        tanBeta = sinThetaI / AbsCosTheta(wi);
//    } else {
//        sinAlpha = sinThetaI;
//        tanBeta = sinThetaO / AbsCosTheta(wo);
//    }
//
//    return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
//}
//
//Color3f OrenNayarBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
//                        Float *pdf, BxDFType *sampledType) const
//{
//    //TODO
//    *wi = WarpFunctions::squareToHemisphereCosine(u);
//    *pdf = OrenNayarBRDF::Pdf(wo,*wi);
//    return OrenNayarBRDF::f(wo,*wi);
//}
//
//float OrenNayarBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
//{
//    //TODO
//    return WarpFunctions::squareToHemisphereCosinePDF(wi);
//}

Color3f LambertBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return R * InvPi;
}

Color3f LambertBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO
    *wi = WarpFunctions::squareToHemisphereCosine(u);
    *pdf = LambertBTDF::Pdf(wo,*wi);
    *wi = -1.f * (*wi);

    return LambertBTDF::f(wo,*wi);
}

float LambertBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return WarpFunctions::squareToHemisphereCosinePDF(wi);
}
