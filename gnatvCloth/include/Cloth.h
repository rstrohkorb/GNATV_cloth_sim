#ifndef CLOTH_H_
#define CLOTH_H_

#include <vector>
#include "MassPoint.h"
#include "Triangle.h"
#include "AbstractClothMaterial.h"

class Cloth
{
public:
    // Explicitly set the default functions
    ~Cloth()=default;                                  // destructor
    Cloth(const Cloth &)=default;                   // copy constructor
    Cloth(Cloth &&)=default;                        // move constructor
    Cloth & operator = (const Cloth &) = default;   // assignment operator
    Cloth & operator = (Cloth &&) = default;        // move assignment operator

    // Delete the default constructor - need a material
    Cloth() = delete;

    // User Constructor
    Cloth(AbstractClothMaterial _material) : m_material(_material) {;}

    // Initializers
    void init(std::string _filename, initOrientation _o);               // inits to obj file

    // Getters/Setters
    size_t numMasses() const { return m_mspts.size(); }
    size_t numTriangles() const { return m_triangles.size(); }
    float mass() const { return m_material.mass(); }
    float firstMass() const { return m_mspts[0].mass(); }

    // Render
    void render(std::vector<ngl::Vec3> &o_vertexData);

    // Run simulation
    void update(float _h);

private:
    // Struct for storing triangle reference points
    struct Triref
    {
        Triangle tri;
        size_t a;
        size_t b;
        size_t c;
    };
    // Member Variables
    std::vector<MassPoint> m_mspts;
    std::vector<Triref> m_triangles;
    AbstractClothMaterial m_material;

    // Helper Functions
    void readObj(std::string _filename);
    void forceCalcPerTriangle(Triref tr);
    ngl::Mat3 vecVecTranspose(ngl::Vec3 _a, ngl::Vec3 _b);
};

#endif
