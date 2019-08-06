/**
 * @file Cloth.h
 * @brief Defines a cloth object based on measured weft/warp/shear properties
 * @author Rachel Strohkorb
*/

#ifndef CLOTH_H_
#define CLOTH_H_

#include <vector>
#include <functional>
#include <boost/math/interpolators/cubic_b_spline.hpp>
#include "MassPoint.h"
#include "Triangle.h"

/**
 * @enum material_type
 * @brief different types of cloth data available for use in Materials.h
*/
enum material_type { WOOL };

/**
 * @class Cloth
 * @brief Stores/operates on a cloth object that derives its internal forces from the
 * current weft/warp/shear state of its triangles
*/
class Cloth
{
public:
    // CONSTRUCTORS/INITIALIZERS
    /*
     * Delete the default constructor - cloth needs a material to be created.
    */
    Cloth() = delete;
    /**
     * @brief user constructor, sets the material properties of the cloth
    */
    Cloth(material_type _mt);
    /**
     * @brief initializes cloth object
     * @param _filename path to an .obj file defining the cloth object.
     * This object must be formed of triangles, and it must have UV texture coordinates.
     * @param _toParam function that converts the triangle vertices from 3D world
     * space to 2d parametric coordinates (more details in Triangle.h)
     * @param _corners Id values for the 'corner' points, or points the user wishes to fix
     * @param _dampingCoefficient the coefficient used for damping in implicit integration
    */
    void init(std::string _filename, std::function<ngl::Vec2(ngl::Vec3)> _toParam,
              std::vector<size_t> _corners, float _dampingCoefficient = 3.0f);

    // GETTERS
    /**
     * @brief returns the current number of masspoints in the cloth object
    */
    size_t numMasses() const { return m_mspts.size(); }
    /**
     * @brief returns the current number of triangles in the cloth object
    */
    size_t numTriangles() const { return m_triangles.size(); }
    /**
     * @brief returns the total mass of the cloth object
    */
    float mass() const { return m_mass; }
    /**
     * @brief returns the mass of the first masspoint
    */
    float firstMass() const { return m_mspts[0].mass(); }
    /**
     * @brief returns the cloth's material
    */
    material_type material() const { return m_material; }
    /**
     * @brief returns the cloth 'corners'
    */
    std::vector<size_t> corners() const { return m_corners; }

    // SPIT OUT VERTEX/TRIANGLE DATA
    /**
     * @brief fills empty list with triangle render data for the cloth
     *
     * The render data is formatted in the following manner:
     *  1. Position of the vertex (3d coord)
     *  2. Normal of the vertex (3d coord)
     *  3. UV coordinates of the vertex (2d coord)
     *  Repeat for each vertex in the triangle
     *  Repeat for all triangles
    */
    void render(std::vector<float> &o_vertexData);
    /**
     * @brief writes out the current cloth state to an obj file
    */
    void writeToObj(std::string _filename);

    // RUN SIMULATION
    /**
     * @brief runs one step of cloth sim
     * @param _h time step
     * @param _externalf any non-gravity external forces acting on the masspoints
    */
    void update(float _h, ngl::Vec3 _externalf);

    // FIX POINT OPERATORS
    /**
     * @brief fixes the 'corners' based on bool list of same/smaller size
     *
     * Whatever 'corners' the user inputted in init will be fixed (or not) in order
     * based on the entries in the _isPtFixed list.
    */
    void fixCorners(std::vector<bool> _isPtFixed);
    /**
     * @brief returns a list of bool values recording whether or not a 'corner' is fixed
    */
    std::vector<bool> isCornerFixed() const;

private:
    // STRUCT
    /**
     * @struct Triref
     * @brief Stores triangle and references to each of its connected masspoints
    */
    struct Triref
    {
        Triangle tri;
        size_t a;
        size_t b;
        size_t c;
    };

    // HELPER FUNCTIONS
    /**
     * @brief reads in data from .obj file, and creates/assigns data to triangles and masspoints
    */
    void readObj(std::string _filename);
    /**
     * @brief resets the forces and jacobians of each masspoint to 0
    */
    void nullForces();
    /**
     * @brief calculates the internal forces acting on the cloth's masspoints
     * @param _h time setp
     * @param _externalf non-gravity external forces acting on the masspoints
     * @param _calcJacobians whether or not the jacobians should be calculated
     * @param _useJvel whether or not the velocity jacobians should be calculated
    */
    void forceCalc(float _h, ngl::Vec3 _externalf, bool _calcJacobians, bool _useJvel = false);
    /**
     * @brief calculates the internal forces acting within a given triangle
     * @param _tr the triangle for which we are calculating the current internal forces
     * @param _calcJacobians whether or not the jacobians should be calculated
     * @param _useJvel whether or not the velocity jacobians should be calculated
    */
    void forceCalcPerTriangle(Triref _tr, bool _calcJacobians, bool _useJvel);
    /**
     * @brief runs implicit integration on the cloth object using the CG method
     * @param _h time step
     * @param _useJvel whether or not the velocity jacobians are being used
     * @param _useDamping whether or not damping is being used
    */
    std::vector<ngl::Vec3> conjugateGradient(float _h, bool _useJvel, bool _useDamping);
    /**
     * @brief runs explicit RK4 integration on the cloth object
     * @param _h time step
     * @param _externalf non-gravity external forces acting on the masspoints
    */
    void rk4Integrate(float _h, ngl::Vec3 _externalf);

    /**
     * @brief calculates current strain based on the warp/weft vectors U and V
    */
    ngl::Vec3 calcStrain(ngl::Vec3 _u, ngl::Vec3 _v);
    /**
     * @brief calculates change in strain based on warp/weft and change in warp/weft
    */
    ngl::Vec3 calcStrainPrime(ngl::Vec3 _u, ngl::Vec3 _up, ngl::Vec3 _v, ngl::Vec3 _vp);
    /**
     * @brief calculates current stress based on strain state
    */
    ngl::Vec3 calcStress(ngl::Vec3 _strain);

    /**
     * @brief computes the position jacobians for the given triangle
     * @param _tr the triangle for which we are computing the position jacobians
     * @param _u current weft direction of the triangle
     * @param _v current warp direction of the triangle
     * @param _strain current strain state of the triangle
     * @param _stress current stress state of the triangle
    */
    void computeJpos(Triref _tr, ngl::Vec3 _u, ngl::Vec3 _v, ngl::Vec3 _strain, ngl::Vec3 _stress);
    /**
     * @brief computes the velocity jacobians for the given triangle
     * @param _tr the triangle for which we are computing the position jacobians
     * @param _u current weft direction of the triangle
     * @param _v current warp direction of the triangle
     *
     * Does not currently work, as the relationship between change in strain and stress needs
     * to be defined in data in order to properly compute df/dv.
    */
    void computeJvel(Triref _tr, ngl::Vec3 _u, ngl::Vec3 _v);
    /**
     * @brief multiplies an nx1 vector of 3x1 vectors by the nxn jacobian matrix stored in masspoints
     * @param _isA whether the operation is using A = M - Jvel - Jpos or just Jpos
     * @param _useJvel whether or not the A matrix should be calculated using Jvel
     * @param _useDamping whether or not damping is turned on
     * @param _h time step
     * @param _vec the nx1 vector of 3x1 vectors to be multiplied by the jacobian
    */
    std::vector<ngl::Vec3> jMatrixMultOp(const bool _isA, const bool _useJvel, const bool _useDamping, float _h, std::vector<ngl::Vec3> _vec);

    /**
     * @brief creates a 3x3 matrix from mutiplying a vector by the transpose of another vector
    */
    ngl::Mat3 vecVecTranspose(ngl::Vec3 _a, ngl::Vec3 _b);
    /**
     * @brief performs a dot product operation on two nx1 vectors composed of 3x1 vectors
    */
    float vecVecDotOp(std::vector<ngl::Vec3> _a, std::vector<ngl::Vec3> _b);
    /**
     * @brief clears near-zero entries in a vector to zero to prevent floating point instability
    */
    ngl::Vec3 cleanNearZero(ngl::Vec3 io_a);
    /**
     * @brief returns vertex normals for each masspoint
    */
    std::vector<ngl::Vec3> calcNormals();

    /**
     * @brief creates the preconditioning matrix needed in the CG method (diag P = 1/diag A)
    */
    std::vector<ngl::Mat3> createPrecon(bool _useJvel, bool _useDamping, float _h);
    /**
     * @brief multiplies the input by the filter matrix that defines fixed properties of masspoints
    */
    void filter(std::vector<ngl::Vec3> &io_vec);

    // MEMBER VARIABLES
    std::vector<MassPoint> m_mspts;     /**< Stores the masspoints */
    std::vector<Triref> m_triangles;    /**< Stores the triangles */
    material_type m_material;           /**< Reference for the cloth's material */

    float m_mass = 0.0f;        /**< Mass of the entire cloth object */
    float m_shearOffset = 0.0f; /**< Starting point of the shear dataset */

    boost::math::cubic_b_spline<float> m_weft;  /**< function for the weft data */
    boost::math::cubic_b_spline<float> m_warp;  /**< function for the warp data */
    boost::math::cubic_b_spline<float> m_shear; /**< function for the shear data */

    std::vector<size_t> m_corners;      /**< This object's 'corners', or the points the user wishes to fix/unfix */
    std::vector<ngl::Mat3> m_filter;    /**< Filter matrix used in CG method for fixed points */
};

#endif
