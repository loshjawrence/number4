#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    //TODO
    float radius = sqrt(sample.x);
        float phi = 2*M_PI * sample.y;
        return glm::vec3(radius * cos(phi), radius * sin(phi), 0.0f);
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    //TODO
    float phi, radius;
        float a = 2*sample.x-1; // (a,b) is now on [-1,1]ˆ2
        float b = 2*sample.y-1;

        if (a > -b) // region 1 or 2
        {
            if (a > b) // region 1, also |a| > |b|
            {
                radius = a;
                phi = (M_PI/4 ) * (b/a);
            }
            else // region 2, also |b| > |a|
            {
                radius = b;
                phi = (M_PI/4) * (2 - (a/b));
            }
        }
        else // region 3 or 4
        {
            if (a < b) // region 3, also |a| >= |b|, a != 0
            {
                radius = -a;
                phi = (M_PI/4) * (4 + (b/a));
            }
            else // region 4, |b| >= |a|, but a==0 and b==0 could occur.
            {
                radius = -b;
                if (b != 0)
                    phi = (M_PI/4) * (6 - (a/b));
                else
                    phi = 0;
            }
        }


        return glm::vec3(radius * cos(phi), radius * sin(phi), 0.0f);
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    //TODO
    return InvPi;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    //TODO
    float z =  1 - 2*sample.x;
        return glm::vec3(sqrt(1 - z*z) *cos(2*M_PI * sample.y), sqrt(1 - z*z)*sin(2*M_PI * sample.y), z);
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    //TODO
    return Inv4Pi;
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    //TODO
    thetaMin = glm::radians(180.f - thetaMin);
    float z = 1.f - (1.f - std::cos(thetaMin)) * sample.x;
    return glm::vec3(sqrt(1 - z*z) *cos(2*M_PI * sample.y), sqrt(1 - z*z)*sin(2*M_PI * sample.y), z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    //TODO
    return Inv2Pi  * (1.0f / (1.0f - cos(glm::radians(180 - thetaMin))));
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    //TODO
    float z =  sample.x;
        return glm::vec3(sqrt(1 - z*z) *cos(2*M_PI * sample.y), sqrt(1 - z*z)*sin(2*M_PI * sample.y), z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    //TODO
    return Inv2Pi;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    //TODO
    glm::vec3 disk = squareToDiskConcentric(sample);
        float x = disk.x;
        float y = disk.y;
        float z = sqrt(1 - x*x - y*y);

        return glm::vec3(x, y, z);
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    //TODO
    float cosTheta = glm::dot(sample, glm::vec3(0,0,1));

       return InvPi * cosTheta;
}
