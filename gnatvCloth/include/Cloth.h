#ifndef CLOTH_H_
#define CLOTH_H_

#include <vector>
#include "MassPoint.h"
#include "Triangle.h"

class Cloth
{
public:
    // Explicitly set all the default functions
    Cloth()=default;                                   // default constructor
    ~Cloth()=default;                                  // destructor
    Cloth(const Cloth &)=default;                   // copy constructor
    Cloth(Cloth &&)=default;                        // move constructor
    Cloth & operator = (const Cloth &) = default;   // assignment operator
    Cloth & operator = (Cloth &&) = default;        // move assignment operator

    // Initializers
    void init(std::string _filename);               // inits to obj file

    // Getters/Setters
    size_t numMasses() const { return m_mspts.size(); }
    size_t numTriangles() const { return m_triangles.size(); }
    float firstMass() const { return m_mspts[0].mass(); }

    // Render
    void render(std::vector<ngl::Vec3> &o_vertexData);

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
    float m_mass = 0.15f; // assuming 1m x 1m cloth, this is 150gsm

    // Helper Functions
    void readObj(std::string _filename);
};

#endif
