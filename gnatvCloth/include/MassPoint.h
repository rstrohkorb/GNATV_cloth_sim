#ifndef MASSPOINT_H_
#define MASSPOINT_H_

#include <unordered_map>
#include <ngl/Vec3.h>
#include <ngl/Mat3.h>

class MassPoint
{
public:
    // Explicitly set all the default functions
    MassPoint()=default;                                    // default constructor
    ~MassPoint()=default;                                   // destructor
    MassPoint(const MassPoint &)=default;                   // copy constructor
    MassPoint(MassPoint &&)=default;                        // move constructor
    MassPoint & operator = (const MassPoint &) = default;   // assignment operator
    MassPoint & operator = (MassPoint &&) = default;        // move assignment operator

    // User Ctor
    MassPoint(ngl::Vec3 _pos, size_t _selfId = 0, float _mass = 1.0f, bool _fixed = false) :
        m_pos(_pos), m_self(_selfId), m_mass(_mass), m_fixed(_fixed) {;}

    // Getters/setters
    ngl::Vec3 pos() const { return m_pos; }
    ngl::Vec3 vel() const { return m_vel; }
    ngl::Vec3 forces() const { return m_forces; }
    float mass() const { return m_mass; }
    bool fixed() const { return m_fixed; }
    size_t numJacobians() const { return m_jacobians.size(); }
    ngl::Mat3 fetchJacobian(size_t _id) { return m_jacobians[_id]; } // not safe, for testing only
    ngl::Vec3 getJacobianDiag();

    void setPos(const ngl::Vec3 _pos) { m_pos = _pos; }
    void setVel(const ngl::Vec3 _vel);
    void setMass(const float _mass) { m_mass = _mass; }
    void setFixed(const bool _isFixed);
    void resetForce() { m_forces = ngl::Vec3(0.0f); }
    void addForce(const ngl::Vec3 _force);

    // Jacobian operators
    bool nullJacobians();                                           // returns true if all the jacobians are zero or empty map
    std::vector<size_t> jacobainKeys() const;                       // fetches all the keys for currently stored jacobians
    void resetJacobians();                                          // zeros out all currently stored jacobians
    void addJacobian(const size_t _id, const ngl::Mat3 _jacobian);  // add this jacobian to the running total for this id
    void multJacobians(const float _hsq);                           // multiply all jacobians by the input value
    ngl::Vec3 jacobianVectorMult(const bool _isA,
                                 std::unordered_map<size_t, ngl::Vec3> _vec);   // multiply the input vector by our jacobian vector

private:
    // member variables
    ngl::Vec3 m_pos = ngl::Vec3(0.0f);
    ngl::Vec3 m_vel = ngl::Vec3(0.0f);
    ngl::Vec3 m_forces = ngl::Vec3(0.0f);
    std::unordered_map<size_t, ngl::Mat3> m_jacobians;
    size_t m_self = 0;
    float m_mass = 1.0f;
    bool m_fixed = false;
};

#endif
