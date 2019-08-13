/**
 * @file MassPoint.h
 * @brief Class defining the mass points in a cloth object
 * @author Rachel Strohkorb
*/

#ifndef MASSPOINT_H_
#define MASSPOINT_H_

#include <unordered_map>
#include <ngl/Vec3.h>
#include <ngl/Mat3.h>

/**
 * @class MassPoint
 * @brief stores/operates on data for MassPoints in cloth sim
*/
class MassPoint
{
public:
    // CONSTRUCTORS
    /**
     * @brief default constructor
    */
    MassPoint()=default;
    /**
     * @brief user constructor
    */
    MassPoint(ngl::Vec3 _pos, size_t _selfId = 0, float _mass = 1.0f, bool _fixed = false, float _damping = 1.0f) :
        m_pos(_pos), m_self(_selfId), m_mass(_mass), m_fixed(_fixed), m_dampingCoefficient(_damping) {;}

    // GETTERS
    /**
     * @brief returns current position
    */
    ngl::Vec3 pos() const { return m_pos; }
    /**
     * @brief returns current velocity
    */
    ngl::Vec3 vel() const { return m_vel; }
    /**
     * @brief returns forces acting on this masspoint
    */
    ngl::Vec3 forces() const { return m_forces; }
    /**
     * @brief returns mass of this point
    */
    float mass() const { return m_mass; }
    /**
     * @brief returns whether or not this point is fixed in space
    */
    bool fixed() const { return m_fixed; }
    /**
     * @brief returns the damping coefficient used in implicit integration
    */
    float dampingCoefficient() const { return m_dampingCoefficient; }
    /**
     * @brief returns current number of jacobians stored
    */
    size_t numJacobians() const { return m_jacobians.size(); }
    /**
     * @brief returns the position jacobian matrix derived from the given masspoint id
     *
     * It doesn't protect against out-of-index ids. Testing is only intended use.
    */
    ngl::Mat3 fetchJpos(size_t _id) { return m_jacobians[_id].Jpos; }
    /**
     * @brief returns the velocity jacobian matrix derived from the given masspoint id
     *
     * It doesn't protect against out-of-index ids. Testing is only intended use.
    */
    ngl::Mat3 fetchJvel(size_t _id) { return m_jacobians[_id].Jvel; }
    /**
     * @brief returns the diagonal values of the position jacobian
     *
     * Diagonal values are the ones stored in the matrix associated with this MassPoint
     * (i.e. id = self_id)
    */
    ngl::Vec3 getJposDiag();
    /**
     * @brief returns the diagonal values of the velocity jacobian
     *
     * Diagonal values are the ones stored in the matrix associated with this MassPoint
     * (i.e. id = self_id)
    */
    ngl::Vec3 getJvelDiag();

    // SETTERS
    /**
     * @brief sets current position
    */
    void setPos(const ngl::Vec3 _pos) { m_pos = _pos; }
    /**
     * @brief sets current velocity
    */
    void setVel(const ngl::Vec3 _vel);
    /**
     * @brief sets the mass of this point
    */
    void setMass(const float _mass) { m_mass = _mass; }
    /**
     * @brief sets whether or not this point is fixed in space
    */
    void setFixed(const bool _isFixed);
    /**
     * @brief sets damping coefficient
    */
    void setDamping(const float _dampingCoefficient) { m_dampingCoefficient = _dampingCoefficient; }

    // OPERATE ON FORCES
    /**
     * @brief reset force vector to 0
    */
    void resetForce() { m_forces = ngl::Vec3(0.0f); }
    /**
     * @brief adds the given force to the total forces acting on this point
    */
    void addForce(const ngl::Vec3 _force);

    // OPERATE ON JACOBIANS
    /**
     * @brief returns true if all the jacobians are empty, or the map is empty
    */
    bool nullJacobians();
    /**
     * @brief fetches all the id keys for currently stored jacobians
    */
    std::vector<size_t> jacobianKeys() const;
    /**
     * @brief zeros out all currently stored jacobians
    */
    void resetJacobians();
    /**
     * @brief adds position jacobian to the running total for the given id
     *
     * If the id does not currently exist in the map, will add an entry at that id
    */
    void addJpos(const size_t _id, const ngl::Mat3 _jpos);
    /**
     * @brief adds velocity jacobian to the running total for the given id
     *
     * If the id does not currently exist in the map, will add an entry at that id
    */
    void addJvel(const size_t _id, const ngl::Mat3 _jvel);
    /**
     * @brief multiply all position jacobians by the input value
    */
    void multJpos(const float _hsq);
    /**
     * @brief multiply all velocity jacobians by the input value
    */
    void multJvel(const float _h);
    /**
     * @brief multiply the input vector by our jacobian matrix
     * @param _isA whether or not the operation uses A = (M - Jvel - Jpos) or just Jpos
     * @param _useJvel whether or not the A matrix should contain the Jvel matrix in its operation
     * @param _useDamping whether or not damping should be used
     * (A = A + nh*damping*I, Jpos = Jpos + h*damping*I)
     * @param _vec the vector by which we are multiplying. It should contain the same map id values
     * as the current jacobian state (all ids not included in the current jacobian state are assumed
     * zero and would have no effect on the multiplication operation)
     * @param _h time step (used if _useDamping is true)
     * @returns vector result of the multiplication operation
    */
    ngl::Vec3 jacobianVectorMult(const bool _isA, const bool _useJvel, const bool _useDamping,
                                 std::unordered_map<size_t, ngl::Vec3> _vec, float _h = 1.0f);

private:
    // STRUCT
    /**
     * @struct Jacobian
     * @brief stores position and velocity Jacobians
    */
    struct Jacobian
    {
        ngl::Mat3 Jpos; /**< position jacobian df/dx */
        ngl::Mat3 Jvel; /**< velocity jacobian df/dv */
    };

    // MEMBER VARIABLES
    ngl::Vec3 m_pos = ngl::Vec3(0.0f);      /**< Position of masspoint */
    ngl::Vec3 m_vel = ngl::Vec3(0.0f);      /**< Velocity of masspoint */
    ngl::Vec3 m_forces = ngl::Vec3(0.0f);   /**< Forces acting on this masspoint */

    /**
     * @brief jacobian matrices associated with this masspoint
     *
     * The ids in the map correspond to which masspoint produced the force
     * partial derivative acting on this masspoint.
    */
    std::unordered_map<size_t, Jacobian> m_jacobians;

    size_t m_self = 0;      /**< Id value associated with this masspoint */
    float m_mass = 1.0f;    /**< Mass of this masspoint */
    bool m_fixed = false;   /**< Whether or not this point is fixed in space */

    float m_dampingCoefficient = 1.0f;  /**< Damping coefficient used in implicit integration */
};

#endif
