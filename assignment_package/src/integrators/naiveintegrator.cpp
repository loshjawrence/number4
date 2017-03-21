#include "naiveintegrator.h"

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, Color3f compounded_energy, int depth) const
{

//    //TODO
//    Color3f color(0.f);

//    Intersection isect = Intersection();
//    if (scene.Intersect(ray,&isect)) {

//        //everything in isect (point, t, b, n ) is in world space
//        Vector3f woW = -1.f * ray.direction;
//        Color3f emittedlightenergy = isect.Le(woW);
//        if (depth == 0 || isect.objectHit->GetMaterial() == nullptr) {
//            return emittedlightenergy;
//        }

//        if(isect.ProduceBSDF()) {
//            Point2f xi = sampler->Get2D();
//            Vector3f wiW(0.f);
//            float pdf = 0;
//            Color3f bsdfresult = isect.bsdf->Sample_f(woW,&wiW,xi,&pdf);
//            bsdfresult /= pdf;
//            Ray wiWray = isect.SpawnRay(wiW);
//            float absdot = std::fabs(glm::dot(wiW,isect.normalGeometric));
//            Color3f recursLipath = Li(wiWray,scene,sampler,compounded_energy,--depth);

//            //LTE
//            color = emittedlightenergy + bsdfresult * recursLipath * absdot;
//        } else {
//            //no material attached to isect, color still 0
//        }
//    }

//    return color;



    if(depth < 0)
        return Color3f(0.f);

    Intersection data;
    if(scene.Intersect(ray, &data))
    {
        Primitive const * p = data.objectHit;
        Vector3f woW = ray.direction * -1.f;

        Color3f color(0.f);

        if(p->ProduceBSDF(&data))
        {
            std::shared_ptr<BSDF> bsdf = data.bsdf;

            Vector2f s = sampler->Get2D();
            float r = sampler->Get1D();

            float pdf = 1.f;
            Vector3f wiW;

            color = bsdf->Sample_f(woW, &wiW, s, &pdf);

            if(pdf != 0.f)
                color /= pdf;

            // Don't continue recursion if light is completely absorbed
            if(glm::length(color) > 0.f)
            {
                color *= AbsDot(wiW, data.normalGeometric);
                color *= Li(data.SpawnRay(wiW), scene, sampler, Color3f(0.f), depth - 1);
            }
        }
        return data.Le(woW) + color;
    }
    return Color3f(0.f);
}
