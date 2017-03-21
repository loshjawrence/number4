#include "fulllightingintegrator.h"

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, Color3f compounded_energy, int depth) const
{
    //TODO
    if(depth <= recursionLimit - 3) {
        float rand = sampler->Get1D();
        float max = compounded_energy.r;
        if ( max < compounded_energy.g) {
            max = compounded_energy.g;
        } else if ( max < compounded_energy.b) {
            max = compounded_energy.b;
        }
        if(max < rand){
            return Color3f(0.f);
        }
    }
    Color3f finalcolor(0.f);
    Color3f DLcolor(0.f);
    Color3f naivecolor(0.f);

    float DLpdf = 0.f;
    float naivepdf = 0.f;

    Intersection isect = Intersection();
    if (scene.Intersect(ray,&isect)) {
        //everything in isect (point, t, b, n ) is in world space
        Vector3f woW = -1.f * ray.direction;
        Color3f emittedlightenergy = isect.Le(woW);
        if (depth == 0 || isect.objectHit->GetMaterial() == nullptr) {
            return emittedlightenergy;
        }

        if(isect.ProduceBSDF()) {
            Point2f xi = sampler->Get2D();
            Vector3f wiW_DL(0.f);

            /*--------------------------------------------------
             * call direct lighting returns lightengery/lightpdf
             * ------------------------------------------------- */
            float temp = xi.x * scene.lights.size();
            xi.x = glm::clamp( (temp - std::floor(temp)) - 0.0001f, 0.f, 1.f) ;
            uint32_t index = temp;

            Color3f sampledlightenergy = scene.lights[index]->Sample_Li(isect,xi,&wiW_DL,&DLpdf);
            if (DLpdf == 0.f) {
                return Color3f(0.f);
            }

            DLpdf /= scene.lights.size();
            sampledlightenergy /= DLpdf;

            Intersection closest = Intersection();
            Ray wiWray_DL = isect.SpawnRay(wiW_DL);
            scene.Intersect(wiWray_DL,&closest);

            if(closest.objectHit == nullptr || closest.objectHit->areaLight != scene.lights.at(index)) {
                sampledlightenergy = Color3f(0.f);
            }
            Color3f bsdfresult_DL = isect.bsdf->f(woW,wiW_DL);
            float absdot_DL = std::fabs(glm::dot(wiW_DL,isect.normalGeometric));

            DLcolor = emittedlightenergy + bsdfresult_DL * sampledlightenergy * absdot_DL;
            Float BxdfPdf_for_DL = isect.bsdf->Pdf(woW, wiW_DL);
            float powerheuristic_DL = PowerHeuristic(1, DLpdf, 1, BxdfPdf_for_DL);
            DLcolor *= powerheuristic_DL;


            /*----------------------------------------------------------------
            * naive lighting integrator: returns bxdf sample f / bxdf pdf
            * -----------------------------------------------------------------*/
            xi = sampler->Get2D();
            Vector3f wiW_naive(0.f);
            Color3f bsdfresult_naive = isect.bsdf->Sample_f(woW,&wiW_naive,xi,&naivepdf);
            compounded_energy *= bsdfresult_naive;
            bsdfresult_naive /= naivepdf;
            Ray wiWray_naive = isect.SpawnRay(wiW_naive);
            float absdot_naive = std::fabs(glm::dot(wiW_naive,isect.normalGeometric));
            Color3f recursLipath = Li(wiWray_naive,scene,sampler,compounded_energy,--depth);

            //LTE
            naivecolor = emittedlightenergy + bsdfresult_naive * recursLipath * absdot_naive;
            Float DLPdf_for_naive = scene.lights[index]->Pdf_Li(isect,wiW_naive);
            float powerheuristic_naive = PowerHeuristic(1, naivepdf, 1, DLPdf_for_naive);
            naivecolor *= powerheuristic_naive;


            finalcolor = DLcolor + naivecolor;
        } else {
            //no material attached to isect, color still 0
        }
    }

    return finalcolor;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    return (nf*fPdf) / (nf*fPdf + ng*gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    float f = nf * fPdf, g = ng * gPdf;
    return (f*f) / (f*f + g*g);
}

