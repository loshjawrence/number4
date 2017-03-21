#include "cube.h"
#include <iostream>

float Cube::Area() const
{
    //TODO
    return (transform.getScale().x * transform.getScale().y + transform.getScale().y * transform.getScale().z + transform.getScale().x * transform.getScale().z) * 2.0f;
}

// Returns +/- [0, 2]
int GetFaceIndex(const Point3f& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i * glm::sign(P[i]);
            val = glm::abs(P[i]);
        }
    }
    return idx;
}

Normal3f GetCubeNormal(const Point3f& P)
{
    int idx = glm::abs(GetFaceIndex(Point3f(P)));
    Normal3f N(0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}


bool Cube::Intersect(const Ray& r, Intersection* isect) const
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return false;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    if(t_n < t_f)
    {
        float t = t_n > 0 ? t_n : t_f;
        if(t < 0)
            return false;
        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t*r_loc.direction, 1);
        InitializeIntersection(isect, t, Point3f(P));
        return true;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return false;
    }
}


void Cube::ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const
{
    Normal3f localNormal = GetCubeNormal(P);
    *nor = glm::normalize(transform.invTransT() * GetCubeNormal(P));
    //TODO: Compute tangent and bitangent


    // +x
    if(localNormal.x == 1.0f)
    {
        *tan = Vector3f(0, 0, -1);
        *bit = Vector3f(0, 1, 0);
    }
    // +z
    else if(localNormal.z == 1)
    {
        *tan = Vector3f(1, 0, 0);
        *bit = Vector3f(0, 1, 0);
    }
    else if(localNormal.x == -1)
    {
        *tan = Vector3f(0, 0, 1);
        *bit = Vector3f(0, 1, 0);
    }
    else if(localNormal.z == -1)
    {
        *tan = Vector3f(-1, 0, 0);
        *bit = Vector3f(0, 1, 0);
    }
    else if(localNormal.y == 1)
    {
        *tan = Vector3f(1, 0, 0);
        *bit = Vector3f(0, 0, -1);
    }
    else if(localNormal.y == -1)
    {
        *tan = Vector3f(1, 0, 0);
        *bit = Vector3f(0, 0, 1);
    }

    glm::vec4 ta = transform.T() * glm::vec4((*tan).r,(*tan).b,(*tan).g,0);
    *tan = glm::normalize(Vector3f(ta.x, ta.y, ta.z));
    //*tan = transform.invTransT() * (*tan);
    *bit = glm::normalize(glm::cross(*nor ,*tan));


}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point) const
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}

Intersection Cube::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    Intersection inter;
    //remapped
    Point2f pt = Point2f(glm::fract(xi.x*3.2345f* xi.y)  -0.5f, xi.y -0.5f);

    std::vector<std::pair<int, float>> faces;
    // std::vector<int> _faces;

    float maxDot = 0.0f;


    //Z+
    glm::vec4 ppt = transform.T() * glm::vec4( pt.x, pt.y, 0.5f, 1.0f);
    if( glm::dot( glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,0,1))) < 0.0f )
    {
        maxDot = -glm::dot( glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,0,1)));
        faces.push_back(std::pair<int, float>(0,maxDot));
    }

    //Z-
    ppt = transform.T() * glm::vec4(pt.x,  pt.y, -0.5f, 1.0f);
    if( glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,0,-1))) < 0.0f )
    {
        maxDot = -glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,0,-1)));
        faces.push_back(std::pair<int, float>(1,maxDot));
    }

    //Y+
    ppt = transform.T() * glm::vec4(pt.x, 0.5f, pt.y, 1.0f);
    if( glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,1,0))) < 0.0f )
    {
        maxDot = -glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,1,0)));
        faces.push_back(std::pair<int, float>(2,maxDot));
    }

    //Y-
    ppt = transform.T() * glm::vec4( pt.x, -0.5f, pt.y, 1.0f);
    if( glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,-1,0))) < 0.0f )
    {
        maxDot = -glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(0,-1,0)));
        faces.push_back(std::pair<int, float>(3,maxDot));
    }

    //X+
    ppt = transform.T() * glm::vec4( 0.5f, pt.x, pt.y, 1.0f);
    if( glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(1,0,0))) < 0.0f )
    {
        maxDot = -glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(1,0,0)));
         faces.push_back(std::pair<int, float>(4,maxDot));
    }

    //X-
    ppt = transform.T() * glm::vec4( -0.5f, pt.x, pt.y, 1.0f);
    if( glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(-1,0,0))) < 0.0f )
    {
        maxDot = -glm::dot(glm::normalize(Point3f(ppt.x, ppt.y, ppt.z) - ref.point), glm::normalize(transform.invTransT() * Normal3f(-1,0,0)));
        faces.push_back(std::pair<int, float>(5,maxDot));
    }


    //Random Select one face
    float totalDot = 0.0f;
    float currentDot = 0.0f;
    for(int i=0; i<faces.size(); i++)
    {
        std::pair<int, float> fuc2k = faces.at(i);

        totalDot += fuc2k.second;
    }

    int chosenFace = -1;

    for(int i=0; i<faces.size(); i++)
    {
         std::pair<int, float> fuc2k = faces.at(i);

        currentDot += fuc2k.second/totalDot;
        if(xi.x <= currentDot)
        {
            chosenFace = i;
            break;
        }
    }

     if(chosenFace < 0)
     {
      *pdf = 0;      
      return inter;
     }

            glm::vec4 WSP;
            Point3f WorldSpacePoint;

            std::pair<int, float> tempPair = faces.at(chosenFace);
                      //Z+

            if(tempPair.first == 0)
            {
                WSP = transform.T() * glm::vec4(pt.x, pt.y, 0.5f,1.0f);
                WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);
                inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
            }

            //Z-
            else if(tempPair.first == 1)
            {
                WSP = transform.T() * glm::vec4(pt.x, pt.y, -0.5f,1.0f);
                WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);
                inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,0,-1));
            }
            //Y+
            else if(tempPair.first == 2)
            {
                WSP = transform.T() * glm::vec4(pt.x, 0.5f, pt.y, 1.0f);
                WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);
                inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,1,0));
            }
            //Y-
            else if(tempPair.first == 3)
            {
                WSP = transform.T() * glm::vec4(pt.x, -0.5f, pt.y,1.0f);
                WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);
                inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0,-1,0));
            }
            //X+
            else if(tempPair.first == 4)
            {
                WSP = transform.T() * glm::vec4(0.5f, pt.x, pt.y, 1.0f);
                WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);
                inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(1,0,0));
            }
            //X-
            else if(tempPair.first == 5)
            {
                WSP = transform.T() * glm::vec4(-0.5f, pt.x, pt.y, 1.0f);
                WorldSpacePoint = Point3f(WSP.x, WSP.y, WSP.z);
                inter.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(-1,0,0));
            }


           inter.point = WorldSpacePoint;

    *pdf = Area() * 1.5f * totalDot;
    return inter;


}

Intersection Cube::Sample(const Point2f &xi, Float *pdf) const
{
    return Intersection();
}
