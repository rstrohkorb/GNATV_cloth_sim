#include <iostream>
#include "MassPoint.h"

ngl::Vec3 MassPoint::getJposDiag()
{
    // handle case of no jacobians
    try
    {
        m_jacobians.at(m_self);
    }
    catch (std::out_of_range)
    {
        return ngl::Vec3(0.0f);
    }
    // get diag
    ngl::Vec3 diag;
    diag.m_x = m_jacobians[m_self].Jpos.m_00;
    diag.m_y = m_jacobians[m_self].Jpos.m_11;
    diag.m_z = m_jacobians[m_self].Jpos.m_22;
    return diag;
}

ngl::Vec3 MassPoint::getJvelDiag()
{
    // handle case of no jacobians
    try
    {
        m_jacobians.at(m_self);
    }
    catch (std::out_of_range)
    {
        return ngl::Vec3(0.0f);
    }
    // get diag
    ngl::Vec3 diag;
    diag.m_x = m_jacobians[m_self].Jvel.m_00;
    diag.m_y = m_jacobians[m_self].Jvel.m_11;
    diag.m_z = m_jacobians[m_self].Jvel.m_22;
    return diag;
}

void MassPoint::setVel(const ngl::Vec3 _vel)
{
    if(!m_fixed)
    {
        m_vel = _vel;
    }
}

void MassPoint::setFixed(const bool _isFixed)
{
    m_fixed = _isFixed;
    if(_isFixed)
    {
        m_vel = ngl::Vec3(0.0f);
        m_forces = ngl::Vec3(0.0f);
    }
}

void MassPoint::addForce(const ngl::Vec3 _force)
{
    //if(!m_fixed)
    {
        m_forces += _force;
    }
}

std::vector<size_t> MassPoint::jacobainKeys() const
{
    std::vector<size_t> keys;
    keys.reserve(m_jacobians.size());
    for(auto pair : m_jacobians)
    {
        keys.push_back(pair.first);
    }
    return keys;
}

bool MassPoint::nullJacobians()
{
    if(m_jacobians.size() == 0)
        return true;
    for(auto pair : m_jacobians)
    {
        if((pair.second.Jpos == ngl::Mat3(0.0f)) &&
                (pair.second.Jvel == ngl::Mat3(0.0f))) continue;
        else
            return false;
    }
    return true;
}

void MassPoint::resetJacobians()
{
    for(auto &pair : m_jacobians)
    {
        // zero out the matrices
        pair.second.Jpos.null();
        pair.second.Jvel.null();
    }
}

void MassPoint::addJpos(const size_t _id, const ngl::Mat3 _jpos)
{
    // make sure initial matrices are zero'd out
    try
    {
        m_jacobians.at(_id);
    }
    catch (std::out_of_range)
    {
        m_jacobians[_id].Jpos = ngl::Mat3(0.0f);
        m_jacobians[_id].Jvel = ngl::Mat3(0.0f);
    }
    // add the jacobian contribution
    m_jacobians[_id].Jpos += _jpos;
}

void MassPoint::addJvel(const size_t _id, const ngl::Mat3 _jvel)
{
    // make sure initial matrices are zero'd out
    try
    {
        m_jacobians.at(_id);
    }
    catch (std::out_of_range)
    {
        m_jacobians[_id].Jpos = ngl::Mat3(0.0f);
        m_jacobians[_id].Jvel = ngl::Mat3(0.0f);
    }
    // add the jacobian contribution
    m_jacobians[_id].Jvel += _jvel;
}

void MassPoint::multJpos(const float _hsq)
{
    for(auto &pair : m_jacobians)
    {
        pair.second.Jpos *= _hsq;
    }
}

void MassPoint::multJvel(const float _h)
{
    for(auto &pair : m_jacobians)
    {
        pair.second.Jvel *= _h;
    }
}

ngl::Vec3 MassPoint::jacobianVectorMult(const bool _isA, const bool _useJvel, const bool _useDamping,
                                        std::unordered_map<size_t, ngl::Vec3> _vec, float _h)
{
    ngl::Vec3 result = ngl::Vec3(0.0f);
    for(auto &pair : m_jacobians)
    {
        ngl::Mat3 jmat = pair.second.Jpos;
        if(_isA)
        {
            jmat *= -1.0f;
            if(_useJvel)
            {
                auto jvel = pair.second.Jvel;
                jvel *= -1.0f;
                jmat += jvel;
            }
            if(pair.first == m_self)
            {
                jmat.m_00 += m_mass;
                jmat.m_11 += m_mass;
                jmat.m_22 += m_mass;
                if(_useDamping)
                {
                    auto n = m_jacobians.size() - 1;
                    jmat.m_00 += (n *_h * m_dampingCoefficient);
                    jmat.m_11 += (n *_h * m_dampingCoefficient);
                    jmat.m_22 += (n *_h * m_dampingCoefficient);
                }
            }

        }
        else
        {
            if(_useDamping && (pair.first == m_self))
            {
                jmat.m_00 += (_h * m_dampingCoefficient);
                jmat.m_11 += (_h * m_dampingCoefficient);
                jmat.m_22 += (_h * m_dampingCoefficient);
            }
        }
        result += jmat * _vec[pair.first];
    }
    return result;
}
