#include "MassPoint.h"

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
