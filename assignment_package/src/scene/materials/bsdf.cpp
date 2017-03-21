#include "bsdf.h"
#include <warpfunctions.h>


BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
//TODO: Properly set worldToTangent and tangentToWorld
    : worldToTangent(Vector3f(isect.tangent.x, isect.bitangent.x, isect.normalGeometric.x),
                     Vector3f(isect.tangent.y, isect.bitangent.y, isect.normalGeometric.y),
                     Vector3f(isect.tangent.z, isect.bitangent.z, isect.normalGeometric.z) ),
      tangentToWorld(isect.tangent, isect.bitangent, isect.normalGeometric),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{}

BSDF::~BSDF() {
    for(int i = 0; i < numBxDFs; ++i) {
        delete bxdfs[i];
    }
}

void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    //TODO: Update worldToTangent and tangentToWorld based on the normal, tangent, and bitangent
    //which space are these in when they are passed to the function
    worldToTangent = Matrix3x3(Vector3f(t.x, b.x, n.x),
                               Vector3f(t.y, b.y, n.y),
                               Vector3f(t.z, b.z, n.z) );
    tangentToWorld = Matrix3x3(t,b,n);
}


// Compute the f() result of all of our BxDFs which match the BxDFType indicated
// Returns the sum of these f()s
// IMPORTANT: Convert woW and wiW from world space to tangent space
// before passing them to BxDF::f()!
//
Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    //TODO
    Color3f sum(0.f);
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT = worldToTangent * wiW;


    for(uint32_t i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i]->MatchesFlags(flags)) {
            sum += bxdfs[i]->f(woT, wiT);
        }
    }

    return sum;
}


// Invokes the Sample_f() of one of our BxDFs, chosen at random from the set
// of BxDFs that match the BxDFType indicated.
// Additionally invokes the f() of all BxDFs after using Sample_f to generate
// a ray direction.
// Refer to the .cpp file for a more detailed function description.

// Use the input random number _xi_ to select
// one of our BxDFs that matches the _type_ flags.

// After selecting our random BxDF, rewrite the first uniform
// random number contained within _xi_ to another number within
// [0, 1) so that we don't bias the _wi_ sample generated from
// BxDF::Sample_f.

// Convert woW and wiW into tangent space and pass them to
// the chosen BxDF's Sample_f (along with pdf).
// Store the color returned by BxDF::Sample_f and convert
// the _wi_ obtained from this function back into world space.

// Iterate over all BxDFs that we DID NOT select above (so, all
// but the one sampled BxDF) and add their PDFs to the PDF we obtained
// from BxDF::Sample_f, then average them all together.

// Finally, iterate over all BxDFs and sum together the results of their
// f() for the chosen wo and wi, then return that sum.
Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType) const
{
//    //TODO
//    std::vector<BxDF*> filtered;
//    for(uint32_t i = 0; i < numBxDFs; ++i) {
//        if(bxdfs[i]->MatchesFlags(type)) {
//            filtered.push_back(bxdfs[i]);
//        }
//    }

//    float temp = xi.x * filtered.size();
//    uint32_t index = temp;
//    const BxDF* const bxdf_for_sampling = filtered[index];

//    Color3f color(0.f);
//    //check if not set already (directlighting will set wiW towards a random light)
////    if( wiW->x == 0.f && wiW->y == 0.f && wiW->z == 0.f) {
//        //Don't want to bias sampling direction based on our chosen bxdf
//        xi.x = glm::clamp( (temp - std::floor(temp)) - 0.0001f, 0.f, 1.f) ;

//        //sample_f and f of bxdf's done in tangent space
//        Vector3f woT = worldToTangent * woW;
//        Vector3f wiT(0.f);

//        //this just for getting a wi
//        color = bxdf_for_sampling->Sample_f(woT, &wiT, xi, pdf, sampledType);
//        *wiW = tangentToWorld * wiT;
////    }
//    float avepdf = 1.f;
//    if( !(bxdf_for_sampling->type & BSDF_SPECULAR) ) {
//        //call BSDF::f to get sum of all BxDFs' f() function
//        //try passing sampledtype here
//        color = BSDF::f(woW, *wiW, type);

//        //call BSDF::Pdf to get average pdf of all BxDFs
//        avepdf = BSDF::Pdf(woW, *wiW, type);
//    } else {
//        avepdf /= filtered.size();
//    }

//    *pdf = avepdf;
//    return color ;


    if(this->numBxDFs == 0)
        return Color3f(0.f);

    float temp = (xi.x * this->numBxDFs);
    int selectedBxDF = (int)temp;
    xi.x = glm::clamp( (temp - std::floor(temp)) - 0.0001f, 0.f, 1.f) ;

    Vector3f tangentWo = worldToTangent * woW;
    Vector3f tangentWi;

    BxDF * bxdf = this->bxdfs[selectedBxDF];
    Color3f bsdfEvaluation = bxdf->Sample_f(tangentWo, &tangentWi, xi, pdf, sampledType);

    // Transform wi back to world
    *wiW = tangentToWorld * tangentWi;

    if(sampledType != nullptr)
        *sampledType = bxdf->type;

    if((bxdf->type & BSDF_SPECULAR) == BSDF_SPECULAR)
    {
        *pdf /= this->numBxDFs;
        return bsdfEvaluation;
    }

    *pdf = Pdf(woW, *wiW, type);

    for(int i = 0; i < this->numBxDFs; i++)
    {
        if(i != selectedBxDF && this->bxdfs[i]->MatchesFlags(type))
            bsdfEvaluation += this->bxdfs[i]->f(tangentWo, tangentWi);
    }

    return bsdfEvaluation;

}


// Computes the average PDF of all BxDFs that match the given BxDFType
// i.e. sum each BxDF's Pdf() result and divide by the number of BxDFs
// that match _flags_.
// Note that _woW_ and _wiW_ are given in world space, and must
// be converted to tangent space before being passed to BxDF::Pdf
float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
    //TODO
    float sum = 0.f;
    int count = 0;
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT = worldToTangent * wiW;


    for(uint32_t i = 0; i < numBxDFs; ++i) {
        if(bxdfs[i]->MatchesFlags(flags)) {
            sum += bxdfs[i]->Pdf(woT, wiT);
            count++;
        }
    }

    return sum / count;
}

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    //TODO
    //this is the default if not overriden(should be): just unif sample on hemisphere since default pdf is for unif sampled hemisphere
    *wi = WarpFunctions::squareToHemisphereUniform(xi);
    *pdf = BxDF::Pdf(wo,*wi);
    return this->f(wo,*wi);
}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //This is the default if not overridden (should be)
    return SameHemisphere(wo, wi) ? Inv2Pi : 0;
}
