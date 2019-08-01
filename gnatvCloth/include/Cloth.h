#ifndef CLOTH_H_
#define CLOTH_H_

#include <vector>
#include <boost/math/interpolators/cubic_b_spline.hpp>
#include "MassPoint.h"
#include "Triangle.h"

enum material_type { WOOL };

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
    Cloth(material_type _mt);

    // Initializers
    void init(std::string _filename, initOrientation _o,
              std::vector<size_t> _corners);               // inits to obj file

    // Getters/Setters
    size_t numMasses() const { return m_mspts.size(); }
    size_t numTriangles() const { return m_triangles.size(); }
    float mass() const { return m_mass; }
    float firstMass() const { return m_mspts[0].mass(); }
    material_type material() const { return m_material; }
    std::vector<size_t> corners() const { return m_corners; }

    // Render
    void render(std::vector<ngl::Vec3> &o_vertexData);

    // Run simulation
    void update(float _h, ngl::Vec3 _externalf);

    // Fixing corners, testing that
    void fixCorners(std::vector<bool> _isPtFixed);
    std::vector<bool> isCornerFixed() const;

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
    material_type m_material;
    float m_mass = 0.0f;
    boost::math::cubic_b_spline<float> m_weft;
    boost::math::cubic_b_spline<float> m_warp;
    boost::math::cubic_b_spline<float> m_shear;
    float m_shearOffset = 0.0f;
    std::vector<size_t> m_corners;
    std::vector<ngl::Mat3> m_filter;

    // Helper Functions
    void readObj(std::string _filename);
    void nullForces();
    void forceCalc(float _h, ngl::Vec3 _externalf, bool _calcJacobians);
    void forceCalcPerTriangle(Triref _tr, bool _calcJacobians);
    std::vector<ngl::Vec3> conjugateGradient(float _h);
    void rk4Integrate(float _h, ngl::Vec3 _externalf);
    ngl::Mat3 vecVecTranspose(ngl::Vec3 _a, ngl::Vec3 _b);
    std::vector<ngl::Vec3> jMatrixMultOp(const bool _isA, std::vector<ngl::Vec3> _vec);
    float vecVecDotOp(std::vector<ngl::Vec3> _a, std::vector<ngl::Vec3> _b);
    ngl::Vec3 cleanNearZero(ngl::Vec3 io_a);
    std::vector<ngl::Mat3> createPrecon();
    void filter(std::vector<ngl::Vec3> &io_vec);
};

#endif
