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
    MassPoint(ngl::Vec3 _pos, float _mass = 1.0f, bool _fixed = false) :
        m_pos(_pos), m_mass(_mass), m_fixed(_fixed) {;}

    // Getters/setters
    ngl::Vec3 pos() const { return m_pos; }
    ngl::Vec3 vel() const { return m_vel; }
    ngl::Vec3 forces() const { return m_forces; }
    float mass() const { return m_mass; }
    bool fixed() const { return m_fixed; }
    size_t numJacobians() const { return m_jacobians.size(); }
    bool nullJacobians();                // returns true if all the jacobians are zero or empty map

    void setMass(const float _mass) { m_mass = _mass; }
    void resetForce() { m_forces = ngl::Vec3(0.0f); }
    void addForce(const ngl::Vec3 _force) { m_forces += _force; }
    void resetJacobians();
    void addJacobian(const size_t _id, const ngl::Mat3 _jacobian);
    ngl::Mat3 fetchJacobian(size_t _id) { return m_jacobians[_id]; } // not safe, for testing only

private:
    // member variables
    ngl::Vec3 m_pos = ngl::Vec3(0.0f);
    ngl::Vec3 m_vel = ngl::Vec3(0.0f);
    ngl::Vec3 m_forces = ngl::Vec3(0.0f);
    std::unordered_map<size_t, ngl::Mat3> m_jacobians;
    float m_mass = 1.0f;
    bool m_fixed = false;
};

#endif
