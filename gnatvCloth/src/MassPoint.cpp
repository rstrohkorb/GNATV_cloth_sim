#include "MassPoint.h"

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
        if(pair.second == ngl::Mat3(0.0f)) continue;
        else
            return false;
    }
    return true;
}

void MassPoint::resetJacobians()
{
    for(auto &pair : m_jacobians)
    {
        // zero out the matrix
        pair.second.null();
    }
}

void MassPoint::addJacobian(const size_t _id, const ngl::Mat3 _jacobian)
{
    // make sure initial matrix is zero'd out
    try
    {
        m_jacobians.at(_id);
    }
    catch (std::out_of_range)
    {
        m_jacobians[_id] = ngl::Mat3(0.0f);
    }
    // add the jacobian contribution
    m_jacobians[_id] += _jacobian;
}

void MassPoint::multJacobians(const float _hsq)
{
    for(auto &pair : m_jacobians)
    {
        pair.second *= _hsq;
    }
}

ngl::Vec3 MassPoint::jacobianVectorMult(const bool _isA, std::unordered_map<size_t, ngl::Vec3> _vec)
{
    ngl::Vec3 result = ngl::Vec3(0.0f);
    for(auto &pair : m_jacobians)
    {
        ngl::Mat3 jmat = pair.second;
        if(_isA)
        {
            jmat *= -1.0f;
            if(pair.first == m_self)
            {
                jmat.m_00 += m_mass;
                jmat.m_11 += m_mass;
                jmat.m_22 += m_mass;
            }
        }
        result += jmat * _vec[pair.first];
    }
    return result;
}
