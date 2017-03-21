#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

float TriArea(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

float Triangle::Area() const
{
    //TODO
    return TriArea(points[0], points[1], points[2]);
}

float Mesh::Area() const
{
    //TODO
    float Area = 0.0f;
    for(int i = 0; i < faces.size(); i++){

        Area += faces.at(i)->Area();
    }

    return Area;
}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = planeNormal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    planeNormal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}


//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
Normal3f Triangle::GetNormal(const Point3f &P) const
{
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], P);
    float A1 = TriArea(points[0], points[2], P);
    float A2 = TriArea(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}


//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
bool Triangle::Intersect(const Ray& r, Intersection* isect) const
{
    //1. Ray-plane intersection
    float t =  glm::dot(planeNormal, (points[0] - r.origin)) / glm::dot(planeNormal, r.direction);
    if(t < 0) return false;

    glm::vec3 P = r.origin + t * r.direction;
    //2. Barycentric test
    float S = 0.5f * glm::length(glm::cross(points[0] - points[1], points[0] - points[2]));
    float s1 = 0.5f * glm::length(glm::cross(P - points[1], P - points[2]))/S;
    float s2 = 0.5f * glm::length(glm::cross(P - points[2], P - points[0]))/S;
    float s3 = 0.5f * glm::length(glm::cross(P - points[0], P - points[1]))/S;
    float sum = s1 + s2 + s3;

    if(s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && fequal(sum, 1.0f)){
        isect->t = t;
        return true;
    }
    return false;
}

Intersection Triangle::Sample(const Point2f &xi, Float *pdf) const
{
    Point2f b = Point2f(1.0f - glm::sqrt(xi.x), xi.y * glm::sqrt(xi.x));
    Intersection it;
    it.point = b.x * points[0] + b.y * points[1] - (1.0f - b.x - b.y) * points[2];

    it.normalGeometric = glm::normalize(Normal3f(glm::cross(points[1] - points[0], points[2] - points[0])));

    *pdf = 1.0f / Area();
    return it;
}

bool Mesh::Intersect(const Ray& r, Intersection* isect) const
{
    //BoundingBoxTest
    //Ray r_bb = r.GetTransformedCopy(BoundingBox.transform.invT());
    Intersection bb_isx;

    if(BoundingBox.Intersect(r, &bb_isx))
    {
        Ray r_loc = r.GetTransformedCopy(transform.invT());
        float closest_t = -1;
        Triangle* closestTri = nullptr;
        for(int i = 0; i < faces.size(); i++){
            Intersection isx;
            if(faces[i]->Intersect(r_loc, &isx)){
                if(isx.t > 0 && (isx.t < closest_t || closest_t < 0)){
                    closest_t = isx.t;
                    closestTri = faces[i];
                }
            }
        }
        if(closest_t > 0)
        {
            Point3f P = Point3f(closest_t * r_loc.direction + r_loc.origin);
            closestTri->InitializeIntersection(isect, closest_t, P);
            this->InitializeIntersection(isect, closest_t, P);
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }

}

void Triangle::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = pLocal;
    isect->uv = GetUVCoordinates(pLocal);
    ComputeTriangleTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent), isect->uv);
    isect->t = t;
}

void Triangle::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    //Triangle uses ComputeTriangleTBN instead of this function.
}

void Triangle::ComputeTriangleTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit, const Point2f &uv) const
{
    *nor = GetNormal(P);
    //TODO: Compute tangent and bitangent based on UV coordinates.


    float e0x = points[1].x - points[0].x;
    float e1x = points[2].x - points[0].x;

    float e0y = points[1].y - points[0].y;
    float e1y = points[2].y - points[0].y;

    float e0z = points[1].z - points[0].z;
    float e1z = points[2].z - points[0].z;

    float u0 = uvs[1].x - uvs[0].x;
    float u1 = uvs[2].x - uvs[0].x;

    float v0 = uvs[1].y - uvs[0].y;
    float v1 = uvs[2].y - uvs[0].y;

    float dino = u0 * v1 - v0 * u1;

    if (dino != 0.0f)
    {
       float r = 1.0f / (dino);
       *tan = glm::normalize( Vector3f((v1 * e0x - v0 * e1x) * r, (v1 * e0y - v0 * e1y) * r, (v1 * e0z - v0 * e1z) * r) );
       *bit = glm::normalize( Vector3f((u0 * e1x - u1 * e0x) * r, (u0 * e1y - u1 * e0y) * r, (u0 * e1z - u1 * e0z) * r)) ;
     }
    else
    {
        *tan = Vector3f(1, 0, 0);
        *bit = Vector3f(0, 1, 0);
    }
}


void Mesh::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    //isect->uv = GetUVCoordinates(pLocal);
    //isect->t = t;
}

void Mesh::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * (*nor));
    //TODO: Transform nor, tan, and bit into world space

    glm::vec4 ta = transform.T() *  glm::vec4((*tan)[0], (*tan)[1], (*tan)[2], 0);
    glm::vec4 bi = transform.T() *  glm::vec4((*bit)[0], (*bit)[1], (*bit)[2], 0);

    *tan = Vector3f(glm::normalize(Vector3f(ta.x, ta.y, ta.z)));
    *bit = Vector3f(glm::normalize(Vector3f(bi.x, bi.y, bi.z)));

}


Point2f Mesh::GetUVCoordinates(const Point3f &point) const
{
    return glm::vec2();
}


Point2f Triangle::GetUVCoordinates(const Point3f &point) const
{
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], point);
    float A1 = TriArea(points[0], points[2], point);
    float A2 = TriArea(points[0], points[1], point);
    return uvs[0] * A0/A + uvs[1] * A1/A + uvs[2] * A2/A;
}

Intersection Mesh::Sample(const Point2f &xi, Float *pdf) const
{
    //Intersect()
    return Intersection();
}

void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        glm::vec3 Min;
        glm::vec3 Max;

        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                if(j == 0)
                {
                   Min = p1;
                   Max = p1;

                   Min = XMVectorMin(Min, p2);
                   Max = XMVectorMax(Max, p2);

                   Min = XMVectorMin(Min, p3);
                   Max = XMVectorMax(Max, p3);

                }
                else
                {
                    Min = XMVectorMin(Min, p1);
                    Max = XMVectorMax(Max, p1);

                    Min = XMVectorMin(Min, p2);
                    Max = XMVectorMax(Max, p2);

                    Min = XMVectorMin(Min, p3);
                    Max = XMVectorMax(Max, p3);
                }

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }

        BoundingBoxCenter = (Min + Max) * 0.5f;
        BoundingBoxExtents = (Max - Min) * 0.5f;

        Transform BBtrans = Transform(BoundingBoxCenter, Vector3f(0.0f), Vector3f(
                                                               (glm::abs(BoundingBoxExtents.x)/0.5f),
                                                               (glm::abs(BoundingBoxExtents.y)/0.5f),
                                                               (glm::abs(BoundingBoxExtents.z)/0.5f))
                                                               );

        BoundingBox.transform.SetWorldTransformMat(this->transform.T() * BBtrans.T());


        std::cout << "" << std::endl;
        //TODO: .mtl file loading
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}
