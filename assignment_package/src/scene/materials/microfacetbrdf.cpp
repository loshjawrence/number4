#include "microfacetbrdf.h"
#include <warpfunctions.h>
Color3f MicrofacetBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    Color3f result(0.f);
    float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
    Vector3f wh = wi + wo;
    if(cosThetaI == 0 || cosThetaO == 0) {
        return result;
    }
    if(wh.x == 0 && wh.y == 0 && wh.z ==0) {
        return result;
    }

    wh = glm::normalize(wh);
    Color3f F = fresnel->Evaluate(glm::dot(wi,wh));

    result = R * distribution->D(wh) * distribution->G(wo,wi) * F /
            (4.f * cosThetaI * cosThetaO);

    return result;
}

Color3f MicrofacetBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    //TODO
    //wh is our reflection normal off the microfacet
    Vector3f wh = distribution->Sample_wh(wo,xi);
    //reflection = -wo + 2 * dot(n, wo), where wh is n here and reflection is wi
    *wi = glm::reflect(-wo,wh);//2.f*glm::dot(wo,wh)*wh - wo;
    *wi = glm::normalize(*wi);
    if(!SameHemisphere(wo,*wi)) {
        return Color3f(0.f);
    }
    *pdf = Pdf(wo,*wi);
    return f(wo,*wi);
}

float MicrofacetBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    if(!SameHemisphere(wo, wi)) {
        return 0.f;
    }
    Vector3f wh = glm::normalize(wi + wo);
    return distribution->Pdf(wo,wh) / (4.f * glm::dot(wo,wh));
}
