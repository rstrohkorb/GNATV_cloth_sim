#ifndef MASSPOINT_H_
#define MASSPOINT_H_

#include <ngl/Vec3.h>

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
    MassPoint(ngl::Vec3 _pos, float _mass, bool _fixed = false) :
        m_pos(_pos), m_mass(_mass), m_fixed(_fixed) {;}

    // Getters/setters
    ngl::Vec3 pos() const { return m_pos; }
    ngl::Vec3 vel() const { return m_vel; }
    ngl::Vec3 forces() const { return m_forces; }
    float mass() const { return m_mass; }
    bool fixed() const { return m_fixed; }

private:
    // member variables
    ngl::Vec3 m_pos = ngl::Vec3(0.0f);
    ngl::Vec3 m_vel = ngl::Vec3(0.0f);
    ngl::Vec3 m_forces = ngl::Vec3(0.0f);
    float m_mass = 1.0f;
    bool m_fixed = false;
};

#endif
