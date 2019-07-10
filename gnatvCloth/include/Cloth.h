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

    // Initializer
    void init();

    // Getters/Setters
    size_t numMasses() const { return m_mspts.size(); }
    size_t numTriangles() const { return m_triangles.size(); }

private:
    // Struct for storing triangle reference points
    struct Triref
    {
        Triangle tri;
        size_t ia;
        size_t ib;
        size_t ic;
    };
    // Member Variables
    std::vector<MassPoint> m_mspts;
    std::vector<Triref> m_triangles;
};

#endif
